// TODO:
// - ACRESCENTAR a memoria flash o reagendamento causado pelo alarme 2

#include <Arduino.h>
#include <DHT.h>

#include "ServidorWeb.h"
#include "ConfiguracaoPersistente.h"
#include "RTC.h"
#include "Controle.h"
#include "Rele.h"
#include "WaterFlow.h"

// ==== Configuracao Servidor Web ====
const char* WIFI_SSID = "Controle Irrigacao";
const char* WIFI_PASSWORD = "123456789";
int numeroAtivacao = 0;



// ==== CONFIG DHT ====
#define DHTPIN 4
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

// ==== OBJETOS GLOBAIS ====
ServidorWeb servidor(WIFI_SSID, WIFI_PASSWORD);
DateTime proximoAcionamento;
ConfiguracaoPersistente configAtual;
RTC rtc;
Rele rele(18);              // Pino do relé
WaterFlow fluxo(2);         // Sensor de fluxo no pino 2
Controle controle(&dht, &rtc, &rele);  // Controle recebe o DHT oficial

// ==== DECLARACAO DA ROTINA ====
void minhaRotinaDeExecucao();

// ==== SETUP ====
void setup() {
    Serial.begin(115200);

    servidor.iniciarAP();
    configAtual.carregar();
    fluxo.iniciar();
    dht.begin();
    rele.iniciar();
    rtc.iniciar();




    // AJUSTE DE HORA PARA TESTES (COMENTAR APÓS O AJUSTE INICIAL)
    rtc.ajustarHorario(2026, 3, 13,
       17, 59, 0); 

    

    
       if (!rtc.iniciar()) {
        Serial.println("Falha ao inicializar o RTC. O agendamento nao funcionará.");
    }

    if (configAtual.getAno() != 0) {
        configAtual.salvarTemporariamente(
            configAtual.getDia(), 
            configAtual.getMes(), 
            configAtual.getAno(), 
            configAtual.getHora(), 
            configAtual.getMinuto(), 
            configAtual.getSegundo(), 
            configAtual.getDuracao(), 
            configAtual.getCiclo());
    }
}

// ==== LOOP ====
void loop() {

    // === 1. LEITURA DO DHT11 (biblioteca Adafruit) ===
    float h = dht.readHumidity();
    float t = dht.readTemperature();

    if (isnan(h) || isnan(t)) {
       //  Serial.println("Falha ao ler DHT!");
    }

    // === 2. ATUALIZAÇÃO DO SENSOR DE FLUXO ===
    fluxo.atualizarCalculo();  
    float vazao = fluxo.getVazao();
    float total = fluxo.getTotal();

    // === 3. HORÁRIO ATUAL VIA RTC ===
    //rtc.atualizarHora();
    DateTime agora = rtc.getAgora();
    String horarioAtual = String(agora.hour()) + ":" +
                          String(agora.minute()) + ":" +
                          String(agora.second());
    
    String dataAtual =
                          String(agora.day()) + "/" +
                          String(agora.month()) + "/" +
                          String(agora.year());

    // === 4. SERVIDOR WEB (passando TODOS os parâmetros) ===
    servidor.manusearClientes(
        rele,
        configAtual,
        t,          // temperatura
        h,          // umidade
        vazao,      // fluxo L/min
        total,      // total irrigado
        horarioAtual,
        dataAtual
    );

    // === 5. ROTINA PRINCIPAL (alarmes, re-agendamentos, logs) ===
    minhaRotinaDeExecucao();

    // Pequeno delay para estabilidade do loop
    delay(10);
}


// ==== ROTINA PRINCIPAL ====
void minhaRotinaDeExecucao() {
    // CANCELAMENTO DE CICLO (se solicitado pela interface web)
    if (configAtual.deveCancelarCiclo()) {
    rele.desligar();
    rtc.cancelarAlarmes();
    configAtual.salvarTemporariamente(0, 0, 0, 0, 0, 0, 0, 0);
    configAtual.salvar();
    configAtual.clearCancelarCiclo();
}
    if (configAtual.isAtualizada()) {
        Serial.println("Detectada nova configuracao. Agendando no RTC...");

        int ciclo = configAtual.getCiclo();
        int cicloHoras = ciclo;  // 0 - unico, 6h, 12h, 18h, 24h

// Se existe uma configuracao valida, agenda o primeiro acionamento
        rtc.agendarAcionamento(
            configAtual.getAno(), 
            configAtual.getMes(), 
            configAtual.getDia(),
            configAtual.getHora(), 
            configAtual.getMinuto(), 
            configAtual.getSegundo(),
            configAtual.getDuracao()
);

        configAtual.clearAtualizada();
    }

    // EVENTO ALARME 1
    if (rtc.alarmeLigou()) {
        Serial.println(">>> ALARME 1 DISPAROU — LIGAR RELE <<<");
        rele.ligar();
        numeroAtivacao++;
    }

    // EVENTO ALARME 2
if (rtc.alarmeDesligou()) {
    Serial.println(">>> ALARME 2 DISPAROU — DESLIGAR RELE <<<");
    rele.desligar();

    int cicloHoras = configAtual.getCiclo();

    if (cicloHoras == 0) {
        Serial.println("Ciclo unico detectado. Nao sera reagendado.");
    } else {
        Serial.print("Reagendando para daqui a ");
        Serial.print(cicloHoras);
        Serial.println(" hora(s)...");

        DateTime agora = rtc.getAgora();
        DateTime proximo = agora + TimeSpan(cicloHoras / 24, cicloHoras % 24, 0, 0);
        proximoAcionamento = proximo;

        rtc.agendarAcionamento(
            proximo.year(),
            proximo.month(),
            proximo.day(),
            proximo.hour(),
            proximo.minute(),
            proximo.second(),
            configAtual.getDuracao()
        );

        Serial.print("Proximo acionamento agendado para: ");
        Serial.print(proximo.day());
        Serial.print("/");
        Serial.print(proximo.month());
        Serial.print("/");
        Serial.print(proximo.year());
        Serial.print(" ");
        Serial.print(proximo.hour());
        Serial.print(":");
        Serial.print(proximo.minute());
        Serial.print(":");
        Serial.println(proximo.second());
    }
}

    // MENSAGENS A CADA 5s
    static long lastMsg = 0;
    if (millis() - lastMsg > 5000) { 
        lastMsg = millis();

        Serial.println("\n----------------------------------------------");

        // FLUXO
        Serial.print("Fluxo: ");
        Serial.print(fluxo.getVazao());
        Serial.print(" L/min | Total: ");
        Serial.print(fluxo.getTotal());
        Serial.println(" L");

        // DHT11 OFICIAL (ADAFRUIT)
        float h = dht.readHumidity();
        float t = dht.readTemperature();

        if (isnan(h) || isnan(t)) {
            Serial.println("Falha ao ler DHT!");
        } else {
            Serial.print("Temp: ");
            Serial.print(t);
            Serial.print(" °C | Umidade: ");
            Serial.print(h);
            Serial.println(" %");
        }

        rtc.mostrarHora();
        Serial.println("Executando rotina principal...");
        configAtual.imprimir(); 
        
        Serial.print("Status da Válvula: "); 
        Serial.println(rele.estaLigado() ? "LIGADO" : "DESLIGADO");
        Serial.println("----------------------------------------------");
        Serial.print("Número de ativações: ");
        Serial.println(numeroAtivacao);

        Serial.print("Próximo acionamento: ");
        Serial.print(proximoAcionamento.day());
        Serial.print("/");
        Serial.print(proximoAcionamento.month());
        Serial.print("/");
        Serial.print(proximoAcionamento.year());
        Serial.print(" ");
        Serial.print(proximoAcionamento.hour());
        Serial.print(":");
        Serial.print(proximoAcionamento.minute());
        Serial.print(":");
        Serial.println(proximoAcionamento.second());
    }
}

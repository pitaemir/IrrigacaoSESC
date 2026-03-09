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
const char* WIFI_SSID = "ESP32-Servidor";
const char* WIFI_PASSWORD = "123456789";

// ==== CONFIG DHT ====
#define DHTPIN 4
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

// ==== OBJETOS GLOBAIS ====
ServidorWeb servidor(WIFI_SSID, WIFI_PASSWORD);
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
    //rtc.ajustarHorario(2025, 11, 27,
    //    11, 47, 0);  // Ajusta para uma data fixa (teste)

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
    DateTime agora = rtc.getNow();
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

    if (configAtual.isAtualizada()) {
        Serial.println("Detectada nova configuracao. Agendando no RTC...");

        String ciclo = configAtual.getCiclo();

        if (ciclo.equalsIgnoreCase("unico") || ciclo.equalsIgnoreCase("diario")) {
            rtc.agendarAcionamento(
                configAtual.getAno(), 
                configAtual.getMes(), 
                configAtual.getDia(),
                configAtual.getHora(), 
                configAtual.getMinuto(), 
                configAtual.getSegundo(),
                configAtual.getDuracao()
            );
        }

        configAtual.clearAtualizada();
    }

    // EVENTO ALARME 1
    if (rtc.alarmeLigou()) {
        Serial.println(">>> ALARME 1 DISPAROU — LIGAR RELE <<<");
        rele.ligar();
    }

    // EVENTO ALARME 2
if (rtc.alarmeDesligou()) {
    Serial.println(">>> ALARME 2 DISPAROU — DESLIGAR RELE <<<");
    rele.desligar();

    String ciclo = configAtual.getCiclo();

    if (ciclo.equalsIgnoreCase("diario")) {
        Serial.println("Ciclo diario detectado. Reagendando para o proximo dia...");

        DateTime agora = rtc.getNow();

        DateTime proximo(
            agora.year(),
            agora.month(),
            agora.day(),
            configAtual.getHora(),
            configAtual.getMinuto(),
            configAtual.getSegundo()
        );

        // Se o horario de hoje ja passou, agenda para amanha
        if (proximo <= agora) {
            proximo = proximo + TimeSpan(1, 0, 0, 0);
        }

        rtc.agendarAcionamento(
            proximo.year(),
            proximo.month(),
            proximo.day(),
            proximo.hour(),
            proximo.minute(),
            proximo.second(),
            configAtual.getDuracao()
        );

        Serial.print("Proximo acionamento diario agendado para: ");
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
    else if (ciclo.equalsIgnoreCase("unico")) {
        Serial.println("Ciclo unico detectado. Nao sera reagendado.");
    }
    else {
        Serial.println("Ciclo desconhecido. Nenhum reagendamento realizado.");
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
    }
}

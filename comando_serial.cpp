/* 
 * Controle de pino via Serial usando DS3231 RTC
 * 
 * Comandos:
 * 1 - Liga o LED (pino 3 em HIGH)
 * 2 - Desliga o LED (pino 3 em LOW)
 * T - Mostra a hora e temperatura atual
 * 
 * Conexões:
 * VCC e GND do RTC conectados à fonte de alimentação
 * SDA, SCL do RTC conectados ao SDA, SCL do Arduino
 */

#include <RTClib.h>

RTC_DS3231 rtc;

#define led_pin 3
char diasDaSemana[7][12] = {"Domingo", "Segunda", "Terca", "Quarta", "Quinta", "Sexta", "Sabado"};

void setup() {
  Serial.begin(9600);
  pinMode(led_pin, OUTPUT);
  digitalWrite(led_pin, LOW);

  // Inicializa o RTC
  if (!rtc.begin()) {
    Serial.println("RTC não encontrado!");
    while (1);
  }

  // Se perdeu energia, ajusta a hora atual para a hora da compilação
  if (rtc.lostPower()) {
    Serial.println("RTC perdeu a hora. Ajustando...");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }

  // Mostra menu de comandos
  Serial.println("\n=== Controle de Pino via Serial ===");
  Serial.println("Comandos disponíveis:");
  Serial.println("1 - Liga o LED (pino 3)");
  Serial.println("2 - Desliga o LED (pino 3)");
  Serial.println("T - Mostra a hora e temperatura atual");
  Serial.println("=====================================");
  
  // Mostra a hora atual na inicialização
  mostrarHoraAtual();
}

// Função para mostrar a hora atual e temperatura
void mostrarHoraAtual() {
  DateTime agora = rtc.now();
  
  Serial.print("Data: ");
  Serial.print(agora.day(), DEC);
  Serial.print('/');
  Serial.print(agora.month(), DEC);
  Serial.print('/');
  Serial.print(agora.year(), DEC);
  Serial.print(" (");
  Serial.print(diasDaSemana[agora.dayOfTheWeek()]);
  Serial.print(") ");
  
  // Formata a hora com zeros à esquerda
  if (agora.hour() < 10) Serial.print("0");
  Serial.print(agora.hour(), DEC);
  Serial.print(':');
  if (agora.minute() < 10) Serial.print("0");
  Serial.print(agora.minute(), DEC);
  Serial.print(':');
  if (agora.second() < 10) Serial.print("0");
  Serial.print(agora.second(), DEC);
  
  Serial.print(" | Temperatura: ");
  Serial.print(rtc.getTemperature());
  Serial.println(" °C");
}

void loop() {
  // Verifica se há dados disponíveis na porta serial
  if (Serial.available() > 0) {
    // Lê o caractere recebido
    char comando = Serial.read();
    
    // Processa o comando
    switch (comando) {
      case '1':
        digitalWrite(led_pin, HIGH);
        Serial.println("LED LIGADO (pino 3 em HIGH)");
        break;
        
      case '2':
        digitalWrite(led_pin, LOW);
        Serial.println("LED DESLIGADO (pino 3 em LOW)");
        break;
        
      case 'T':
      case 't':
        mostrarHoraAtual();
        break;
        
      case '\n':
      case '\r':
        // Ignora caracteres de nova linha
        break;
        
      default:
        Serial.println("Comando inválido. Use 1, 2 ou T.");
        break;
    }
  }
  
  // Pequeno delay para estabilidade
  delay(50);
}
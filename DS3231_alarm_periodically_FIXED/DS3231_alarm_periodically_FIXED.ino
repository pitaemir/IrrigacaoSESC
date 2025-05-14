/* Example implementation of an alarm using DS3231
 *
 * VCC and GND of RTC should be connected to some power source
 * SDA, SCL of RTC should be connected to SDA, SCL of arduino
 * SQW should be connected to CLOCK_INTERRUPT_PIN
 * CLOCK_INTERRUPT_PIN needs to work with interrupts
 */

#include <RTClib.h>
// #include <Wire.h>

RTC_DS3231 rtc;

// the pin that is connected to SQW
#define CLOCK_INTERRUPT_PIN 2
#define led_pin 3

volatile bool alarmeDisparado = false;
char diasDaSemana[7][12] = {"Domingo", "Segunda", "Terca", "Quarta", "Quinta", "Sexta", "Sabado"}; //Dias da semana

// Função que trata a interrupção
void tratarInterrupcaoAlarm1() {
  alarmeDisparado = true;
}

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

  // Desativa saída 32kHz e alarmes antigos
  rtc.disable32K();
  rtc.clearAlarm(1);
  rtc.clearAlarm(2);
  rtc.disableAlarm(2);
  rtc.writeSqwPinMode(DS3231_OFF); // Garante que INT/SQW está no modo interrupção

  // Configura pino da interrupção e função de tratamento
  pinMode(CLOCK_INTERRUPT_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(CLOCK_INTERRUPT_PIN), tratarInterrupcaoAlarm1, FALLING);

  // Programa o primeiro alarme para exatamente 1 minuto após o horário atual
  DateTime agora = rtc.now();
  DateTime alarme = agora + TimeSpan(0, 0, 1, 0); // +1 minuto
  
  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  rtc.setAlarm1(alarme, DS3231_A1_Date);

  Serial.println("Sistema iniciado!");
  Serial.print("Primeiro alarme agendado para: ");
  imprimirDataHora(alarme);
}

// Função para imprimir data e hora formatadas
void imprimirDataHora(DateTime momento) {
  Serial.print("Data: ");
  Serial.print(momento.day(), DEC);
  Serial.print('/');
  Serial.print(momento.month(), DEC);
  Serial.print('/');
  Serial.print(momento.year(), DEC);
  Serial.print(" / Dia da semana: ");
  Serial.print(diasDaSemana[momento.dayOfTheWeek()]);
  Serial.print(" / Hora: ");
  Serial.print(momento.hour(), DEC);
  Serial.print(':');
  Serial.print(momento.minute(), DEC);
  Serial.print(':');
  Serial.print(momento.second(), DEC);
  Serial.print(" / Temperatura: ");
  Serial.print(rtc.getTemperature());
  Serial.println(" *C");
}

void loop() {
  // Usando apenas a flag da interrupção para evitar execução duplicada
  if (rtc.alarmFired(1)) { // alarmeDisparado || rtc.alarmFired(1)
    alarmeDisparado = false;
    rtc.clearAlarm(1);

    // Ativa o LED por 5 segundos (simula irrigação)
    digitalWrite(led_pin, HIGH);
    Serial.println("LED ligado - Irrigação ativada");
    delay(5000);
    digitalWrite(led_pin, LOW);
    Serial.println("LED desligado - Irrigação finalizada");

    // Reprograma o alarme para exatamente 1 minuto após o horário atual
    DateTime agora = rtc.now();
    DateTime proximoAlarme = agora + TimeSpan(0, 0, 1, 0);
    rtc.setAlarm1(proximoAlarme, DS3231_A1_Date);

    Serial.print("Novo alarme agendado para: ");
    imprimirDataHora(proximoAlarme);
  }

  delay(1000); // Polling leve
}
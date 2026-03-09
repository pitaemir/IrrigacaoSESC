#include <Arduino.h>

const int relePin = 18;

void setup() {
  Serial.begin(115200);
  delay(1000);

  pinMode(relePin, OUTPUT);
  digitalWrite(relePin, LOW); // começa desligado

  Serial.println("Teste simples de acionamento do relé (GPIO 18)");
}

void loop() {

  Serial.println("Relé LIGADO");
  digitalWrite(relePin, HIGH);
  delay(2000);

  Serial.println("Relé DESLIGADO");
  digitalWrite(relePin, LOW);
  delay(2000);

}
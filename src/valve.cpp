#include "config.h"
#include "globals.h"

void setValveState (bool state) {
  if (state) {
    digitalWrite(debugLedPin, HIGH); // Liga a válvula (simulado pelo LED)
    Serial.println("Válvula aberta.");
    flowStateFlag = true; // sensor de vazao , quando for true, iremos ler o sensor

  } else {
    digitalWrite(debugLedPin, LOW); // Desliga a válvula (simulado pelo LED)
    Serial.println("Válvula fechada.");
    flowStateFlag = false; // para de ler o sensor de vazao
  }
}
#include "ControladorValvula.h"
#include <Arduino.h> // Para pinMode, digitalWrite, Serial.println

// Construtor: Inicializa o pino e o estado
ControladorValvula::ControladorValvula(int valvulaPin, int ledPin) 
    : pin(valvulaPin), estado("off"), ledPin(ledPin) {
    
    pinMode(pin, OUTPUT);
    digitalWrite(pin, LOW);
    pinMode(ledPin, OUTPUT);
}

void ControladorValvula::ligar() {
    digitalWrite(pin, HIGH);
    estado = "on";
    Serial.println("Válvula LIGADA");
}

void ControladorValvula::desligar() {
    digitalWrite(pin, LOW);
    estado = "off";
    Serial.println("Válvula DESLIGADA");
}

String ControladorValvula::getEstado() const {
    return estado;
}
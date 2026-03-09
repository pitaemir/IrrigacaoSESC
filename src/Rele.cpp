#include "Rele.h"

Rele::Rele(uint8_t pino) {
    this->pino = pino;
    ligado = false;
}

void Rele::iniciar() {
    pinMode(pino, OUTPUT);
    desligar();  // garante que começa desligado
    Serial.println("Módulo de relé iniciado.");
}

void Rele::ligar() {
    digitalWrite(pino, HIGH);  // depende do módulo: HIGH ou LOW ativa
    ligado = true;
    Serial.println("Relé LIGADO (irrigação ativada).");
}

void Rele::desligar() {
    digitalWrite(pino, LOW);
    ligado = false;
    Serial.println("Relé DESLIGADO (irrigação parada).");
}

bool Rele::estaLigado() {
    return ligado;
}

void Rele::testar() {
    Serial.println("Testando relé...");
    ligar();
    delay(1000);
    desligar();
    delay(1000);
    Serial.println("Teste concluído.");
}

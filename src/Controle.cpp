#include "Controle.h"

Controle::Controle(DHT* sensorDHT, RTC* rtc, Rele* rele) {
    this->sensorDHT = sensorDHT;
    this->rtc       = rtc;
    this->rele      = rele;
    temperatura     = 0.0;
    umidade         = 0.0;
}

// Lê a temperatura E a umidade em uma única chamada (como deve ser)
void Controle::lerTemperatura() {

    float u = sensorDHT->readHumidity();
    float t = sensorDHT->readTemperature();

    if (isnan(u) || isnan(t)) {
        // Serial.println("Falha ao ler DHT!");
        return;
    }

    umidade     = u;
    temperatura = t;

    Serial.print("Temperatura: ");
    Serial.print(temperatura);
    Serial.println("°C");
}

void Controle::lerUmidade() {
    Serial.print("Umidade: ");
    Serial.print(umidade);
    Serial.println("%");
}

void Controle::controlarIrrigacao() {
    if (umidade < 40.0) {
        Serial.println("Umidade baixa, irrigando...");
        rele->ligar();
    } else {
        Serial.println("Umidade adequada, desligando irrigação.");
        rele->desligar();
    }
}

void Controle::testarSistema() {
    Serial.println("=== Teste de Sistema ===");

    lerTemperatura();
    lerUmidade();
    rtc->mostrarHora();
    controlarIrrigacao();
}

void Controle::exibirStatus() {
    Serial.print("Status atual: Temp=");
    Serial.print(temperatura);
    Serial.print("°C  Umid=");
    Serial.print(umidade);
    Serial.println("%");
}

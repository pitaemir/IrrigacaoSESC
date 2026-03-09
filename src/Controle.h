#ifndef CONTROLE_H
#define CONTROLE_H

#include <Arduino.h>
#include <DHT.h>

#include "RTC.h"
#include "Rele.h"

class Controle {
private:
    DHT* sensorDHT;   // ponteiro para sensor DHT (Adafruit)
    RTC* rtc;
    Rele* rele;

    float temperatura;
    float umidade;

public:
    Controle(DHT* sensorDHT, RTC* rtc, Rele* rele);

    void lerTemperatura();  // lê tudo
    void lerUmidade();      // só imprime

    void controlarIrrigacao();
    void testarSistema();
    void exibirStatus();

    float getTemperatura() { return temperatura; }
    float getUmidade()     { return umidade; }
};

#endif

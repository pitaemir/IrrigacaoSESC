#include "WaterFlow.h"

WaterFlow::WaterFlow(uint8_t pino) 
    : pino(pino), pulsos(0), vazao_LporMin(0), total_L(0) {}

void IRAM_ATTR fluxoISR(void* arg) {
    WaterFlow* sensor = static_cast<WaterFlow*>(arg);
    sensor->contarPulso();
}

void WaterFlow::iniciar() {
    pinMode(pino, INPUT_PULLUP);

    // attachInterruptArg é obrigatório no ESP32
    attachInterruptArg(pino, fluxoISR, this, RISING);

    pulsos = 0;
    vazao_LporMin = 0;
    total_L = 0;
}

void WaterFlow::contarPulso() {
    pulsos++;
}

void WaterFlow::atualizarCalculo() {
    static unsigned long ultimoTempo = 0;
    unsigned long agora = millis();

    if (agora - ultimoTempo >= 1000) {  // a cada 1s
        float pulsosPorSegundo = pulsos;
        pulsos = 0;
        ultimoTempo = agora;

        // Fórmula padrão do YFS201B
        vazao_LporMin = pulsosPorSegundo / 7.5;

        // Converter L/min → L/s e acumular
        total_L += vazao_LporMin / 60.0;
    }
}

float WaterFlow::getVazao() {
    return vazao_LporMin;
}

float WaterFlow::getTotal() {
    return total_L;
}

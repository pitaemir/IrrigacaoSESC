#ifndef WATERFLOW_H
#define WATERFLOW_H

#include <Arduino.h>

class WaterFlow {
private:
    uint8_t pino;
    volatile uint32_t pulsos;
    float vazao_LporMin;
    float total_L;

public:
    WaterFlow(uint8_t pino);

    void iniciar();
    void contarPulso();          // ISR
    void atualizarCalculo();     // converter pulsos → litros/min
    float getVazao();            // L/min
    float getTotal();            // litros acumulados
};

#endif

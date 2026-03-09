#ifndef RELE_H
#define RELE_H

#include <Arduino.h>

class Rele {
private:
    uint8_t pino;   // pino conectado ao módulo de relé
    bool ligado;    // guarda o estado atual

public:
    Rele(uint8_t pino);  // construtor

    void iniciar();       // configura o pino como saída
    void ligar();         // aciona o relé
    void desligar();      // desliga o relé
    bool estaLigado();    // retorna true se estiver ligado
    void testar();        // faz um teste rápido de acionamento
};

#endif

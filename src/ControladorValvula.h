#ifndef CONTROLADORVALVULA_H
#define CONTROLADORVALVULA_H

#include <Arduino.h> // Para pinMode, digitalWrite e String

/**
 * @brief Classe para controlar o pino da válvula e seu estado.
 */
class ControladorValvula {
private:
    const int pin;
    String estado; // "on" ou "off"
    const int ledPin; // Para o LED Built-in

public:
    // Construtor
    ControladorValvula(int valvulaPin, int ledPin);

    // Métodos de Controle e Acesso
    void ligar();
    void desligar();
    String getEstado() const;
};

#endif // CONTROLADORVALVULA_H
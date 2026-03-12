#ifndef CONFIGURACAO_H
#define CONFIGURACAO_H

#include <Arduino.h> // Para a classe String

/**
 * @brief Classe para gerenciar a programação de acionamento.
 */
class Configuracao {
private:
    int dia = 0;
    int mes = 0;
    int ano = 0;
    int hora = 0;
    int minuto = 0;
    int segundo = 0;
    int duracao = 0; // Duração em minutos
    int ciclo = 0;   // Ciclo em horas (0 = único, >0 = repetição a cada X horas)
public:
    // Construtor (vazio, pois usa valores padrão)
    Configuracao() {}

    // Métodos para controle e acesso
    void salvar(int d, int m, int a, int h, int min, int s, int dur, int c);
    void imprimir();

    // Getters
    int getAno() const { return ano; }
    int getCiclo() const { return ciclo; }
};

#endif // CONFIGURACAO_H
#include "Configuracao.h"
#include <Arduino.h> // Para Serial.print

/**
 * @brief Salva os dados de configuração de acionamento.
 */
void Configuracao::salvar(int d, int m, int a, int h, int min, int s, int dur, String c) {
    dia = d;
    mes = m;
    ano = a;
    hora = h;
    minuto = min;
    segundo = s;
    duracao = dur;
    ciclo = c;
}

/**
 * @brief Imprime a configuração salva no Monitor Serial.
 */
void Configuracao::imprimir() {
    Serial.println("==============================================");
    if (ano != 0) {
        Serial.println("Configuracao de Acionamento Salva:");
        Serial.print("Data Programada: "); Serial.print(dia); Serial.print("/"); 
        Serial.print(mes); Serial.print("/"); Serial.println(ano);
        Serial.print("Hora Programada: "); Serial.print(hora); Serial.print(":"); 
        Serial.print(minuto); Serial.print(":"); Serial.println(segundo);
        Serial.print("Duracao: "); Serial.print(duracao); Serial.println(" minutos");
        Serial.print("Ciclo: "); Serial.println(ciclo);
    } else {
        Serial.println("Nenhuma configuracao salva ainda.");
    }
    Serial.println("==============================================");
}
#include "RTC.h"
#include <Arduino.h>

RTC::RTC() {}

bool RTC::iniciar() {
    if (!rtc.begin()) {
        Serial.println("RTC não encontrado!");
        Serial.flush();
        return false;
    }

    // Só ajusta se o RTC realmente perdeu a alimentação/horário
    if (rtc.lostPower()) {
        Serial.println("RTC perdeu a hora. Ajustando...");
        atualizarHora();
    }

    rtc.disable32K();
    rtc.clearAlarm(1);
    rtc.clearAlarm(2);
    rtc.writeSqwPinMode(DS3231_OFF);
    rtc.disableAlarm(2);

    return true;
}

void RTC::atualizarHora() {
    agora = rtc.now();
}

void RTC::mostrarHora() {
    atualizarHora();

    char buffer[40];
    sprintf(buffer, "Hora atual: %02d:%02d:%02d",
            agora.hour(), agora.minute(), agora.second());
    Serial.println(buffer);
    char buffer2[40];
    sprintf(buffer2, "Data atual: %02d/%02d/%04d",
            agora.day(), agora.month(), agora.year());
    Serial.println(buffer2);
}

String RTC::horaFormatada() {
    atualizarHora();

    char buffer[10];
    sprintf(buffer, "%02d:%02d:%02d",
            agora.hour(), agora.minute(), agora.second());

    return String(buffer);
}

bool RTC::horarioProgramado(int horaAlvo, int minutoAlvo) {
    atualizarHora();
    return (agora.hour() == horaAlvo && agora.minute() == minutoAlvo);
}

void RTC::agendarAcionamento(int ano, int mes, int dia,
                             int hora, int minuto, int segundo,
                             int duracaoMinutos) {
    horarioOn = DateTime(ano, mes, dia, hora, minuto, segundo);
    horarioOff = horarioOn + TimeSpan(0, 0, duracaoMinutos, 0);

    rtc.setAlarm1(horarioOn, DS3231_A1_Hour);
    rtc.setAlarm2(horarioOff, DS3231_A2_Hour);

    Serial.println();
    Serial.println("=== Alarme configurado ===");

    char buffer1[80];
    sprintf(buffer1, "Ligar:    %02d/%02d/%04d %02d:%02d:%02d",
            horarioOn.day(), horarioOn.month(), horarioOn.year(),
            horarioOn.hour(), horarioOn.minute(), horarioOn.second());
    Serial.println(buffer1);

    char buffer2[80];
    sprintf(buffer2, "Desligar: %02d/%02d/%04d %02d:%02d:%02d",
            horarioOff.day(), horarioOff.month(), horarioOff.year(),
            horarioOff.hour(), horarioOff.minute(), horarioOff.second());
    Serial.println(buffer2);
}

void RTC::ajustarParaHoraDoComputador() {
    DateTime novoHorario(F(__DATE__), F(__TIME__));
    rtc.adjust(novoHorario);

    Serial.println("\n=== RTC ajustado para a hora do computador ===");

    char buffer[50];
    sprintf(buffer, "Nova hora: %02d/%02d/%04d %02d:%02d:%02d",
            novoHorario.day(), novoHorario.month(), novoHorario.year(),
            novoHorario.hour(), novoHorario.minute(), novoHorario.second());
    Serial.println(buffer);
}

void RTC::ajustarHorario(int ano, int mes, int dia,
                         int hora, int minuto, int segundo) {
    DateTime novoHorario(ano, mes, dia, hora, minuto, segundo);
    rtc.adjust(novoHorario);

    Serial.println("\n=== Horário do RTC atualizado ===");

    char buffer[50];
    sprintf(buffer, "Novo horario: %02d/%02d/%04d %02d:%02d:%02d",
            novoHorario.day(), novoHorario.month(), novoHorario.year(),
            novoHorario.hour(), novoHorario.minute(), novoHorario.second());
    Serial.println(buffer);
}

void RTC::agendarCicloDiario(int horaInicial, int minutoInicial, int repeticoes) {
    DateTime agoraLocal = rtc.now();

    int ano = agoraLocal.year();
    int mes = agoraLocal.month();
    int dia = agoraLocal.day();

    int intervaloHoras = 24 / repeticoes;

    // Primeiro acionamento do dia
    horarioOn = DateTime(ano, mes, dia, horaInicial, minutoInicial, 0);
    horarioOff = horarioOn + TimeSpan(0, 0, 1, 0);  // 1 minuto de duração

    rtc.setAlarm1(horarioOn, DS3231_A1_Hour);
    rtc.setAlarm2(horarioOff, DS3231_A2_Hour);

    Serial.println("\n=== CICLO DIÁRIO CONFIGURADO ===");

    for (int i = 0; i < repeticoes; i++) {
        DateTime lig = horarioOn + TimeSpan(0, intervaloHoras * i, 0, 0);
        DateTime des = lig + TimeSpan(0, 0, 1, 0);

        char buff[80];
        sprintf(buff, "Ciclo %d -> Ligar: %02d:%02d  |  Desligar: %02d:%02d",
                i + 1,
                lig.hour(), lig.minute(),
                des.hour(), des.minute());
        Serial.println(buff);
    }
}

bool RTC::alarmeLigou() {
    if (rtc.alarmFired(1)) {
        rtc.clearAlarm(1);
        return true;
    }
    return false;
}

bool RTC::alarmeDesligou() {
    if (rtc.alarmFired(2)) {
        rtc.clearAlarm(2);
        return true;
    }
    return false;
}

void RTC::cancelarAlarmes() {
    rtc.clearAlarm(1);
    rtc.clearAlarm(2);
    Serial.println("Alarmes do RTC cancelados.");
}
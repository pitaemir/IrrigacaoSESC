#ifndef RTC_CLASS_H
#define RTC_CLASS_H

#include <RTClib.h>

class RTC {
private:
    RTC_DS3231 rtc;

    DateTime horarioOn;
    DateTime horarioOff;
    DateTime agora;



public:
    RTC();

    bool iniciar();
    void atualizarHora();
    void mostrarHora();
    String horaFormatada();

    bool horarioProgramado(int horaAlvo, int minutoAlvo);

    // NOVAS FUNÇÕES
    void agendarAcionamento(int ano, int mes, int dia,
                            int hora, int minuto, int segundo,
                            int duracaoMinutos);

    void ajustarParaHoraDoComputador();
    void cancelarAlarmes();

    void ajustarHorario(int ano, int mes, int dia,
        int hora, int minuto, int segundo);

    void agendarCicloDiario(int horaInicial, int minutoInicial, int repeticoes);


            

    bool alarmeLigou();
    bool alarmeDesligou();

    DateTime getHorarioOn() { return horarioOn; }
    DateTime getHorarioOff() { return horarioOff; }
    DateTime getAgora() { return agora; }
};

#endif

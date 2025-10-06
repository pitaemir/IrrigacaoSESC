#include "rtc_utils.h"
#include "globals.h"
#include "rtc_utils.h"
#include "globals.h"
#include "config.h"
#include "valve.h"

void pulseCounter()
{
  pulseCount++;
  // Serial.println("Pulses: " + String(pulseCount));
  unsigned long currentTime;
  unsigned long elapsedTime;

  // digitalWrite(debugLedPin2, !digitalRead(debugLedPin2));
  currentTime = millis();
  elapsedTime = currentTime - oldTime;

  if (elapsedTime > 5000)
  {
    // Calcula a vazão
    flowRate = (1000.0 / (elapsedTime)) * pulseCount;

    // Resetar contadores
    pulseCount = 0;
    oldTime = currentTime;

    // Calcula a quantidade de água passada
    flowMilliLitres = (flowRate / 60) * 1000;

    // Adiciona à quantidade total
    totalMilliLitres += flowMilliLitres;
  }
}

void imprimirDataHora(DateTime momento)
{
  Serial.print("Data: ");
  Serial.print(momento.day(), DEC);
  Serial.print('/');
  Serial.print(momento.month(), DEC);
  Serial.print('/');
  Serial.print(momento.year(), DEC);
  Serial.print(" / Dia da semana: ");
  Serial.print(diasDaSemana[momento.dayOfTheWeek()]);
  Serial.print(" / Hora: ");
  if (momento.hour() < 10)
    Serial.print("0");
  Serial.print(momento.hour(), DEC);
  Serial.print(':');
  if (momento.minute() < 10)
    Serial.print("0");
  Serial.print(momento.minute(), DEC);
  Serial.print(':');
  if (momento.second() < 10)
    Serial.print("0");
  Serial.print(momento.second(), DEC);
  Serial.print(" / Temperatura: ");
  Serial.print(myRTC.getTemperature());
  Serial.println(" °C");
  Serial.println();
}

void onAlarm()
{
  Serial.println("Alarm occurred!");
  alarmFiredFlag = true;
  

}

void printDateTime(DateTime scheduledTime, Ds3231Alarm1Mode mode)
{
  char alarm1Date[12] = "DD hh:mm:ss";
  scheduledTime.toString(alarm1Date);
  Serial.print("[Alarm1: ");
  Serial.print(alarm1Date);
  Serial.print(", Mode: ");
  switch (mode)
  {
  case DS3231_A1_PerSecond:
    Serial.print("PerSecond");
    break;
  case DS3231_A1_Second:
    Serial.print("Second");
    break;
  case DS3231_A1_Minute:
    Serial.print("Minute");
    break;
  case DS3231_A1_Hour:
    Serial.print("Hour");
    break;
  case DS3231_A1_Date:
    Serial.print("Date");
    break;
  case DS3231_A1_Day:
    Serial.print("Day");
    break;
  }
  Serial.println("]");
}

void printDateTime(DateTime scheduledTime, Ds3231Alarm2Mode mode)
{
  char alarm2Date[12] = "DD hh:mm:ss";
  scheduledTime.toString(alarm2Date);
  Serial.print("[Alarm2: ");
  Serial.print(alarm2Date);
  Serial.print(", Mode: ");
  switch (mode)
  {
  case DS3231_A1_PerSecond:
    Serial.print("PerSecond");
    break;
  case DS3231_A1_Second:
    Serial.print("Second");
    break;
  case DS3231_A1_Minute:
    Serial.print("Minute");
    break;
  case DS3231_A1_Hour:
    Serial.print("Hour");
    break;
  case DS3231_A1_Date:
    Serial.print("Date");
    break;
  case DS3231_A1_Day:
    Serial.print("Day");
    break;
  }
  Serial.println("]");
}

void scheduleAlarm(int year, int month, int day, int hour, int minute, int second, int cycle)
{
  alarm1Time = DateTime(year, month, day, hour, minute, second);
  alarm2Time = alarm1Time + TimeSpan(0, 0, fbDuration, 0);
  myRTC.setAlarm1(alarm1Time, DS3231_A1_Minute);
  myRTC.setAlarm2(alarm2Time, DS3231_A2_Minute);

  Serial.println("\n--- Próximo ciclo agendado ---");
  Serial.printf("Duração configurada: %d minutos\n", fbDuration);

  Serial.printf("Ligar às: %02d:%02d:%02d em %02d/%02d/%04d\n",
                alarm1Time.hour(), alarm1Time.minute(), alarm1Time.second(),
                alarm1Time.day(), alarm1Time.month(), alarm1Time.year());

  Serial.printf("Desligar às: %02d:%02d:%02d em %02d/%02d/%04d\n",
                alarm2Time.hour(), alarm2Time.minute(), alarm2Time.second(),
                alarm2Time.day(), alarm2Time.month(), alarm2Time.year());

  Serial.println("-------------------------------------");

  DateTime nextAlarm;

  if (cycle == 1) {
    // Diário → soma 1 dia
    nextAlarm = alarm1Time + TimeSpan(1, 0, 0, 0);
  } else if (cycle == 2) {
    // Semanal → soma 7 dias
    nextAlarm = alarm1Time + TimeSpan(7, 0, 0, 0);
  } else {
    // Caso inválido, padrão: 1 dia
    nextAlarm = alarm1Time + TimeSpan(1, 0, 0, 0);
  }

  Serial.println("\n--- Próxima repetição calculada ---");
  Serial.printf("Próximo acionamento em: %02d/%02d/%04d às %02d:%02d:%02d\n",
                nextAlarm.day(), nextAlarm.month(), nextAlarm.year(),
                nextAlarm.hour(), nextAlarm.minute(), nextAlarm.second());
  Serial.println("-------------------------------------");

  alarm1Time = nextAlarm;

}

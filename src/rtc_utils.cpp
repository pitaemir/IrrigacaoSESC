#include "rtc_utils.h"
#include "globals.h"
#include "rtc_utils.h"
#include "globals.h"

void pulseCounter() {
  pulseCount++;
}

void imprimirDataHora(DateTime momento) {
  Serial.print("Data: ");
  Serial.print(momento.day(), DEC);
  Serial.print('/');
  Serial.print(momento.month(), DEC);
  Serial.print('/');
  Serial.print(momento.year(), DEC);
  Serial.print(" / Dia da semana: ");
  Serial.print(diasDaSemana[momento.dayOfTheWeek()]);
  Serial.print(" / Hora: ");
  if (momento.hour() < 10) Serial.print("0");
  Serial.print(momento.hour(), DEC);
  Serial.print(':');
  if (momento.minute() < 10) Serial.print("0");
  Serial.print(momento.minute(), DEC);
  Serial.print(':');
  if (momento.second() < 10) Serial.print("0");
  Serial.print(momento.second(), DEC);
  Serial.print(" / Temperatura: ");
  Serial.print(myRTC.getTemperature());
  Serial.println(" °C");
}

void onAlarm() {
  Serial.println("Alarm occurred!");
  alarmFiredFlag = true;
}

void printDateTime(DateTime scheduledTime, Ds3231Alarm1Mode mode) {
  char alarm1Date[12] = "DD hh:mm:ss";
  scheduledTime.toString(alarm1Date);
  Serial.print("[Alarm1: ");
  Serial.print(alarm1Date);
  Serial.print(", Mode: ");
  switch (mode) {
    case DS3231_A1_PerSecond: Serial.print("PerSecond"); break;
    case DS3231_A1_Second: Serial.print("Second"); break;
    case DS3231_A1_Minute: Serial.print("Minute"); break;
    case DS3231_A1_Hour: Serial.print("Hour"); break;
    case DS3231_A1_Date: Serial.print("Date"); break;
    case DS3231_A1_Day: Serial.print("Day"); break;
  }
  Serial.println("]");
}

void printDateTime(DateTime scheduledTime, Ds3231Alarm2Mode mode) {
  char alarm2Date[12] = "DD hh:mm:ss";
  scheduledTime.toString(alarm2Date);
  Serial.print("[Alarm2: ");
  Serial.print(alarm2Date);
  Serial.print(", Mode: ");
  switch (mode) {
    case DS3231_A1_PerSecond: Serial.print("PerSecond"); break;
    case DS3231_A1_Second: Serial.print("Second"); break;
    case DS3231_A1_Minute: Serial.print("Minute"); break;
    case DS3231_A1_Hour: Serial.print("Hour"); break;
    case DS3231_A1_Date: Serial.print("Date"); break;
    case DS3231_A1_Day: Serial.print("Day"); break;
  }
  Serial.println("]");
}

void scheduleAlarm(int year, int month, int day, int hour, int minute, int second) {
  alarm1Time = DateTime(year, month, day, hour, minute, second);
  alarm2Time = alarm1Time + TimeSpan(0, 0, fbDuration, 0);
  myRTC.setAlarm1(alarm1Time, DS3231_A1_Minute);
  myRTC.setAlarm2(alarm2Time, DS3231_A2_Minute);

  Serial.println("\n--- Próximo ciclo agendado ---");
  Serial.print("Próximo Alarme 1 (LIGA): ");
  imprimirDataHora(alarm1Time);
  Serial.print("Próximo Alarme 2 (DESLIGA): ");
  imprimirDataHora(alarm2Time);
}

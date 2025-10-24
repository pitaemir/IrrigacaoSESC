#pragma once
#include <RTClib.h>

void imprimirDataHora(DateTime momento);
void scheduleAlarm(int year, int month, int day, int hour, int minute, int second, int cycle);
void onAlarm();
void printDateTime(DateTime scheduledTime, Ds3231Alarm1Mode mode);
void printDateTime(DateTime scheduledTime, Ds3231Alarm2Mode mode);

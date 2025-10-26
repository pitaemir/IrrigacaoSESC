#pragma once
#include <Arduino.h>
#include <RTClib.h>
#include <Adafruit_BME280.h>
#include <Firebase_ESP_Client.h>
#include <Preferences.h>
#include "DHT.h"
#include "config.h"

extern TaskHandle_t taskHandle1, taskHandle2, taskHandle3, taskHandle4;

// RTC
extern RTC_DS3231 myRTC;
extern volatile bool alarmFiredFlag;
extern volatile bool flowStateFlag;
extern char diasDaSemana[7][12];
extern DateTime alarm1Time, alarm2Time;

// Firebase
extern FirebaseData fbdo;
extern FirebaseAuth auth;
extern FirebaseConfig config;
extern bool signupOK;
extern String watchPath;
extern String watchPath2;

// Variáveis globais de horário e temperatura do Firebase
extern int fbYear, fbMonth, fbDay, fbHour, fbMinute, fbSecond, fbDuration, fbCycle;

// Sensor de fluxo
extern volatile unsigned int pulseCount;
extern float flowRate;
extern unsigned int flowMilliLitres;
extern unsigned long totalMilliLitres;
extern unsigned long oldTime;
extern float DHTtemp;
extern float flowData[2];
extern float RTCtemp;
extern float avg_temp;
extern struct ConfigData {
    int year;
    int month;
    int day;
    int hour;
    int minute;
    int second;
    int cycle;
    int duration;
} configData;
extern bool dataReadyToSend;
extern ConfigData alarmTime;

//DHT

// BME
extern Adafruit_BME280 bme;

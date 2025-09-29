#pragma once

// ===== WiFi =====
#define WIFI_SSID "JR"
#define WIFI_PASSWORD "22070000"
//#define WIFI_SSID "LARM"
//#define WIFI_PASSWORD "LarmUfsc2022"

// ===== Firebase =====
#define API_KEY "AIzaSyA1LI1y7m9T-av1WW1sDQMSdqyaG6RtDFU"
#define DATABASE_URL "https://hortasesc-9b067-default-rtdb.firebaseio.com/"

// ===== Pinos =====
const int debugLedPin =         19;   //pino de saida do sinal
const int debugLedPin2 =        18;
const int debugLedPin3 =        5;
const int ClockInterruptPin =   4;    // GPIO onde o pino INT/SQW do DS3231 está conectado
const int sensorPin =           2;    //pino de entrada do sensor de fluxo

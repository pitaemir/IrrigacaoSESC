#include "globals.h"

TaskHandle_t taskHandle1, taskHandle2, taskHandle3, taskHandle4;

RTC_DS3231 myRTC;
volatile bool alarmFiredFlag = false;
volatile bool flowStateFlag = false; // variável para controlar a leitura do sensor de fluxo
char diasDaSemana[7][12] = {"Domingo", "Segunda", "Terca", "Quarta", "Quinta", "Sexta", "Sabado"};

DateTime alarm1Time = DateTime(2025, 9, 19, 17, 54, 0);
DateTime alarm2Time = DateTime(2025, 9, 5, 16, 6, 0);

FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;
bool signupOK = false;
String watchPath = "/test";

// Variáveis globais para armazenar a data/hora vinda do Firebase

int fbYear =        2025;
int fbMonth =       1;
int fbDay =         1;
int fbHour =        0;
int fbMinute =      0;
int fbSecond =      0;
int fbDuration =    1; //duração em minutos

volatile unsigned int pulseCount =  0;         // Variável para contar pulsos
float flowRate =                    0.0;       // Vazão em litros por minuto
unsigned int flowMilliLitres =      0;         // Quantidade de água passada (em mililitros)
unsigned long totalMilliLitres =    0;         // Total de água passada (em mililitros)
unsigned long oldTime =             0;         // Armazena o tempo anterior

Adafruit_BME280 bme;

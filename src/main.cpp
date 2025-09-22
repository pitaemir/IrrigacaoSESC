#include <WiFi.h>
#include <WebServer.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <Wire.h>
#include "RTClib.h"
#include <Adafruit_BME280.h>
#include <Firebase_ESP_Client.h>
#include <addons/TokenHelper.h> // for token generation helper functions
#include <addons/RTDBHelper.h> // for Realtime Database helper functions
#include <SPIFFS.h> // Biblioteca para manipulação do sistema de arquivos SPIFFS
#include <Preferences.h> // Biblioteca para armazenamento não volátil

// Wifi credentials
#define WIFI_SSID "iPhone de Samady"
#define WIFI_PASSWORD "sepamadyn"

// Firebase project API Key
#define API_KEY "AIzaSyA1LI1y7m9T-av1WW1sDQMSdqyaG6RtDFU"
#define DATABASE_URL "https://hortasesc-9b067-default-rtdb.firebaseio.com/"

// FREE RTOS Debug Variables
const int debugLedPin = 19; // pino de saida do sinal
const int debugLedPin2 = 18;
const int debugLedPin3 = 5;
TaskHandle_t taskHandle1;
TaskHandle_t taskHandle2;
TaskHandle_t taskHandle3;
TaskHandle_t taskHandle4;
// RTC Variables
const int ClockInterruptPin = 4; // GPIO onde o pino INT/SQW do DS3231 está conectado
volatile bool alarmFiredFlag = false; // Flag para indicar que o alarme foi disparado
char diasDaSemana[7][12] = {"Domingo", "Segunda", "Terca", "Quarta", "Quinta", "Sexta", "Sabado"};  // (year, month, day, hour, minutes, seconds)
DateTime alarm1Time = DateTime(2025, 9, 5, 16, 4, 0);
DateTime alarm2Time = DateTime(2025, 9, 5, 16, 6, 0);
RTC_DS3231 myRTC;
// Water Flow Sensor Variables
const int sensorPin = 2;          // Pino onde o sensor de fluxo está conectado   
volatile unsigned int pulseCount; // Variável para contar pulsos
float flowRate;                   // Vazão em litros por minuto
unsigned int flowMilliLitres;     // Quantidade de água passada (em mililitros)
unsigned long totalMilliLitres;   // Total de água passada (em mililitros)
unsigned long oldTime;            // Armazena o tempo anterior
//BME280 Variables
Adafruit_BME280 bme;
// Firebase Variables
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;
bool signupOK = false;
String watchPath = "/test"; // Variable to track


// FUNCTIONS PROTOTYPES
void Task1(void *pvParameters);  // Task que lida com o disparo do alarme
void Task2(void *pvParameters);  // Task de monitoramento do sistema
void Task3(void *pvParameters);  // Task que lida com o sensor de fluxo de água
void Task4(void *pvParameters);  // Task que lida com o armazenamento local dos dados

void onAlarm(); // Função chamada quando o alarme dispara

void pulseCounter(); // Função de interrupção para contar pulsos do sensor de fluxo

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
  return;
}

void onAlarm() {
  Serial.println("Alarm occurred!");
  alarmFiredFlag = true;
  return;
}

void printDateTime(DateTime scheduledTime, Ds3231Alarm1Mode mode) {
  // Get Details about the alarm1
  char alarm1Date[12] = "DD hh:mm:ss";
  // Mask the output of date and time into the char array
  scheduledTime.toString(alarm1Date);
  // Print the alarm details
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
  Serial.print("]");
  Serial.println("");

  return;
}

void printDateTime(DateTime scheduledTime, Ds3231Alarm2Mode mode) {
  // Get Details about the alarm1
  char alarm2Date[12] = "DD hh:mm:ss";
  // Mask the output of date and time into the char array
  scheduledTime.toString(alarm2Date);
  // Print the alarm details
  Serial.print("[Alarm1: ");
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
  Serial.print("]");
  Serial.println("");

  return;
}

void scheduleAlarm(int year, int month, int day, int hour, int minute, int second) {
  
  DateTime alarm1Time = DateTime(year, month, day, hour, minute, second);

  DateTime alarm2Time = alarm1Time + TimeSpan(0, 0, 1, 0);

  // Configura os alarmes
  myRTC.setAlarm1(alarm1Time, DS3231_A1_Minute);
  myRTC.setAlarm2(alarm2Time, DS3231_A2_Minute);
  
  Serial.println("\n--- Próximo ciclo agendado ---");
  Serial.print("Próximo Alarme 1 (LIGA): ");
  imprimirDataHora(alarm1Time);
  Serial.print("Próximo Alarme 2 (DESLIGA): ");
  imprimirDataHora(alarm2Time);
  return;
}

// Interrupção para contar pulsos
void pulseCounter() {
  pulseCount++;
  return;
}

// Callback called when the tracked variable changes
void streamCallback(FirebaseStream data) {
  Serial.println("=== STREAM EVENT ===");
  Serial.printf("Stream Path: %s\n", data.streamPath().c_str());
  Serial.printf("Type: %s\n", data.dataType().c_str());
  Serial.printf("Data: %s\n", data.stringData().c_str());

  // true = on
  // false = off
  if (data.dataTypeEnum() == fb_esp_rtdb_data_type_json) {
    Serial.println(">>> Recebido um objeto JSON para agendamento.");

    // Crie um objeto JSON para analisar os dados
    FirebaseJson json;
    json.setJsonData(data.stringData());

    int year, month, day, hour, minute, second;

    // Use o método get para extrair os valores
    // O valor é armazenado em uma FirebaseJsonData
    FirebaseJsonData jsonData;
        
    json.get(jsonData, "year");
    year = jsonData.intValue;
    Serial.println("year = " + String(year));

    json.get(jsonData, "month");
    month = jsonData.intValue;
    Serial.println("month = " + String(month));

    json.get(jsonData, "day");
    day = jsonData.intValue;
    Serial.println("day = " + String(day));

    json.get(jsonData, "hour");
    hour = jsonData.intValue;
    Serial.println("hour = " + String(hour));

    json.get(jsonData, "minute");
    minute = jsonData.intValue;
    Serial.println("minute = " + String(minute));

    json.get(jsonData, "second");
    second = jsonData.intValue;
    Serial.println("second = " + String(second));

    // Chame vetor para agendar o alarme
  } else if (data.dataTypeEnum() == fb_esp_rtdb_data_type_boolean) {
    bool val = data.boolData();

    if (val) {
      Serial.println(">>> CONTROL = true (ON)");
      digitalWrite(debugLedPin, HIGH);
    } else {
      Serial.println(">>> CONTROL = false (OFF)");
      digitalWrite(debugLedPin, LOW);
    }
  } else if (data.dataTypeEnum() == fb_esp_rtdb_data_type_integer) {
    int val = data.intData();
    String path = data.dataPath();

    Serial.println(">>> Recebido um valor inteiro!");
    
    // Tratamento específico para o caso de um valor direto em "/test"
    if (path == "/test") {
      Serial.println(">>> Alerta: O valor '" + String(val) + "' foi recebido diretamente no caminho '/test'.");
      Serial.println(">>> Isso pode ter sobrescrito todo o objeto JSON, apagando as chaves internas (year, month, etc.).");
      return; // Interrompe a função para evitar processamento incorreto
    }

    // A partir daqui, o código só lida com sub-chaves
    String subKey = path.substring(path.lastIndexOf('/') + 1);
    Serial.println(">>> Sub-chave identificada: '" + subKey + "'");

    if (subKey == "year") {
        Serial.println(">>> Chave 'year' atualizada para: " + String(val));
        // int year = val;
    } else if (subKey == "month") {
        Serial.println(">>> Chave 'month' atualizada para: " + String(val));
        // int month = val;
    } else if (subKey == "day") {
        Serial.println(">>> Chave 'day' atualizada para: " + String(val));
        // int day = val;
    } else if (subKey == "hour") {
        Serial.println(">>> Chave 'hour' atualizada para: " + String(val));
        // int hour = val;
    } else if (subKey == "minute") {
        Serial.println(">>> Chave 'minute' atualizada para: " + String(val));
        // int minute = val;
    } else if (subKey == "second") {
        Serial.println(">>> Chave 'second' atualizada para: " + String(val));
        // int second = val;
    } else {
        Serial.println(">>> Chave desconhecida '" + subKey + "' com valor: " + String(val));
    }
  }

  //scheduleAlarm(vetor);
  Serial.println("====================");
  return;
}

void streamTimeoutCallback(bool timeout) {
  if (timeout) {
    Serial.println("Stream timeout, reconectando...");
  }
}

void setup() {
  Serial.begin(115200);
  Serial.println("Iniciando Setup...");

  pinMode(debugLedPin, OUTPUT);
  pinMode(debugLedPin2, OUTPUT);
  pinMode(debugLedPin3, OUTPUT);             
  pinMode(sensorPin, INPUT);                // Pino que recebe sinal do sensor de fluxo
  pinMode(ClockInterruptPin, INPUT_PULLUP); // Configura o pino de interrupção como entrada com pull-up
  digitalWrite(debugLedPin, LOW);
  digitalWrite(debugLedPin2, HIGH);
  digitalWrite(debugLedPin3, HIGH);
  digitalWrite(sensorPin, HIGH);


  // Setup Servidor Wifi
  // Inicializa servidor
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to WIFI");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());

  // Setup Firebase
  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;

  if (Firebase.signUp(&config, &auth, "", "")) {
    Serial.println("signUp OK");
    signupOK = true;
  } else {
    Serial.printf("signUp failed: %s\n", config.signer.signupError.message.c_str());
  }

  config.token_status_callback = tokenStatusCallback;

  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);

  // Inicia stream para monitorar alterações no caminho
  if (!Firebase.RTDB.beginStream(&fbdo, watchPath.c_str())) {
    Serial.printf("beginStream failed: %s\n", fbdo.errorReason().c_str());
  } else {
    Firebase.RTDB.setStreamCallback(&fbdo, streamCallback, streamTimeoutCallback);

    Serial.println("Ouvindo mudanças em " + watchPath);
  }

  // Setup RTC
  // Inicializa o RTC
  if (! myRTC.begin()) {
    Serial.println("RTC não encontrado!");
    Serial.flush();
    while (1) delay(10);
  }

  myRTC.adjust(DateTime(F(__DATE__), F(__TIME__)));

  // Se perdeu energia, ajusta a hora atual para a hora da compilação
  if (myRTC.lostPower()) {
    Serial.println("RTC perdeu a hora. Ajustando...");
    myRTC.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }

  attachInterrupt(digitalPinToInterrupt(ClockInterruptPin), onAlarm, FALLING);

  // Desativa saída 32kHz e alarmes antigos
  myRTC.disable32K();
  myRTC.clearAlarm(1);
  myRTC.clearAlarm(2);
  myRTC.writeSqwPinMode(DS3231_OFF); // Garante que INT/SQW está no modo interrupção
  myRTC.disableAlarm(2);

  // Setup BME280
  bool status;
  status = bme.begin(0x76);  
  if (!status) {
    Serial.println("Could not find a valid BME280 sensor, check wiring!");
    while (1);
  }

  // Setup WATER FLOW SENSOR
  pulseCount = 0;
  flowRate = 0.0;
  flowMilliLitres = 0;
  totalMilliLitres = 0;
  oldTime = 0;
  attachInterrupt(digitalPinToInterrupt(sensorPin), pulseCounter, FALLING);

  // Setup Tasks do FreeRTOS
  //Serial.println("Criando tasks do FreeRTOS...");
  //xTaskCreatePinnedToCore(
  //  Task1,
  //  "Task1",
  //  4096,
  //  NULL,
  //  1,
  //  &taskHandle1,
  //  0
  //);

  //xTaskCreatePinnedToCore(
  //  Task2,
  //  "Task2",
  //  4096,
  //  NULL,
  //  2,
  //  &taskHandle2,
  //  1
  //);

  //xTaskCreatePinnedToCore(
  //  Task3,
  //  "Task3",
  //  4096,
  //  NULL,
  //  2,
  //  &taskHandle3,
  //  1
  //);

  //xTaskCreatePinnedToCore(
  //  Task4,
  //  "Task4",
  //  4096,
  //  NULL,
  //  1,
  //  &taskHandle4,
  //  1
  //);
  Serial.println("Tasks criadas. Setup concluído.");
}

void loop() {
  //vTaskDelay(pdMS_TO_TICKS(5000));
}

void Task1(void *pvParameters) {
  Serial.println("Task1: Iniciada no Core " + String(xPortGetCoreID()));
  Serial.println("Task1: GPIO " + String(debugLedPin) + " configurado como OUTPUT para piscar LED.");

  if(!myRTC.setAlarm1(alarm1Time, DS3231_A1_Minute)) {  // this mode triggers the alarm when the minutes match
    Serial.println("Error, alarm wasn't set!");
  }else {
    Serial.println("Alarm 1 will happen at specified time");
  }

  printDateTime(myRTC.getAlarm1(), myRTC.getAlarm1Mode());

  while (true) {
    if(alarmFiredFlag){
      if (myRTC.alarmFired(1)) {
        myRTC.clearAlarm(1);
        Serial.println(" - Alarm 1 cleared");
        
        // Set Alarm 2
        if(!myRTC.setAlarm2(alarm2Time, DS3231_A2_Minute)) {  // this mode triggers the alarm when the minutes match
          Serial.println("Error, alarm wasn't set!");
        }else {
          Serial.println("Alarm 2 will happen at specified time");
        }
        printDateTime(myRTC.getAlarm2(), myRTC.getAlarm2Mode());
        digitalWrite(debugLedPin, HIGH);
      }else if (myRTC.alarmFired(2)){
        myRTC.clearAlarm(2);
        Serial.println(" - Alarm 2 cleared");
        digitalWrite(debugLedPin, LOW);
      }
      alarmFiredFlag = false;
    }
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}

void Task2(void *pvParameters) {
  Serial.println("Task2: Iniciada no Core " + String(xPortGetCoreID()));
  Serial.println("Task2: GPIO " + String(debugLedPin2) + " configurado como OUTPUT para piscar LED.");

  while (true) {
    Serial.println("Task2");
    digitalWrite(debugLedPin2, !digitalRead(debugLedPin2));
    //imprimirDataHora(myRTC.now());
    Serial.print("Temperature = ");
    Serial.print(bme.readTemperature());
    Serial.println(" *C");
    Serial.print("Pressure = ");
    Serial.print(bme.readPressure() / 100.0F);
    Serial.println(" hPa");
    vTaskDelay(pdMS_TO_TICKS(5000));
  }
}


void Task3(void *pvParameters) {
  Serial.println("Task3: Iniciada no Core " + String(xPortGetCoreID()));
  Serial.println("Task2: GPIO " + String(sensorPin) + " configurado como INPUT para receber interrupcao.");
  unsigned long currentTime;
  unsigned long elapsedTime;


  while (true) {
    digitalWrite(debugLedPin2, !digitalRead(debugLedPin2));
    currentTime = millis();
    elapsedTime = currentTime - oldTime;

    if (elapsedTime > 5000) {
      // Calcula a vazão
      flowRate = (1000.0 / (elapsedTime)) * pulseCount;
  
      // Resetar contadores
      pulseCount = 0;
      oldTime = currentTime;
  
      // Calcula a quantidade de água passada
      flowMilliLitres = (flowRate / 60) * 1000;
  
      // Adiciona à quantidade total
      totalMilliLitres += flowMilliLitres;
  
      // Imprime os resultados
      Serial.print("Vazao: ");
      Serial.print(flowRate);
      Serial.print(" L/min - ");
      Serial.print("Quantidade de agua: ");
      Serial.print(flowMilliLitres);
      Serial.print(" mL/segundo - Total: ");
      Serial.print(totalMilliLitres);
      Serial.println(" mL");
    }
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}

void Task4(void *pvParameters) {
  Serial.println("Task3: Iniciada no Core " + String(xPortGetCoreID()));
  Serial.println("Task2: GPIO " + String(sensorPin) + " configurado como INPUT para receber interrupcao.");

  while (true) {
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}

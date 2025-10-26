#include <Arduino.h>
#include <WiFi.h>
#include <SPIFFS.h>
#include "config.h"
#include "globals.h"
#include "rtc_utils.h"
#include "firebase_utils.h"
#include "tasks.h"
#include <addons/TokenHelper.h>
#include <addons/RTDBHelper.h>
#include "valve.h"
#include "dataStoraging.h"
#include "sensors.h"

const unsigned long FETCH_INTERVAL_MS = 120 * 1000;
const unsigned long PRINT_INTERVAL_MS = 5 * 1000;
const unsigned long SEND_INTERVAL_MS = 45 * 1000;

unsigned long lastFetchTime = 0;
unsigned long lastPrintTime = 0;
unsigned long lastSendTime = 0;

void readSensors(float &temp, float &flow, long &total) {
    // Simulação: gera valores aleatórios para demonstrar.
    temp = 20.0 + (random(0, 500) / 100.0); // Temperatura entre 20.0 e 25.0
    flow = random(0, 150) / 10.0;           // Fluxo entre 0.0 e 15.0

    // Para o total, vamos apenas incrementá-lo para simular acúmulo.
    static long accumulatedML = 0;
    if (flow > 0) {
        accumulatedML += (long)flow * 10; // Simula um acúmulo
    }
    total = accumulatedML;
}

void setup()
{
  Serial.begin(115200);
  Serial.println("Iniciando Setup...");

  pinMode(debugLedPin, OUTPUT);
  pinMode(debugLedPin2, OUTPUT);
  pinMode(debugLedPin3, OUTPUT);
  pinMode(sensorPin, INPUT);                // Pino que recebe sinal do sensor de fluxo
  pinMode(ClockInterruptPin, INPUT_PULLUP); // Configura o pino de interrupção como entrada com pull-up

  attachInterrupt(digitalPinToInterrupt(ClockInterruptPin), onAlarm, FALLING);
  attachInterrupt(digitalPinToInterrupt(sensorPin), pulseCounter, FALLING); // interrupcao externa. sempre que variar o pino D2, vai entrar aqui . 

  // WiFi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to WIFI");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());

  // Firebase
  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;

  if (Firebase.signUp(&config, &auth, "", "")) {
      signupOK = true;
  } else {
      Serial.printf("signUp failed: %s\n", config.signer.signupError.message.c_str());
  }

  config.token_status_callback = tokenStatusCallback;
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);

  // RTC
  if (!myRTC.begin()){
    Serial.println("RTC não encontrado!");
    Serial.flush();
    /* while (1)
      delay(10); */
  } else{
    myRTC.adjust(DateTime(F(__DATE__), F(__TIME__)));

    // Se perdeu energia, ajusta a hora atual para a hora da compilação
    if (myRTC.lostPower()){
      Serial.println("RTC perdeu a hora. Ajustando...");
      myRTC.adjust(DateTime(F(__DATE__), F(__TIME__)));
    }

    myRTC.disable32K();
    myRTC.clearAlarm(1);
    myRTC.clearAlarm(2);
    myRTC.writeSqwPinMode(DS3231_OFF);
    myRTC.disableAlarm(2);
  }

  // Inicializa o sistema de arquivos SPIFFS.
  // O parâmetro 'true' formata o SPIFFS se a montagem falhar (útil na primeira execução).
  if (!SPIFFS.begin(true)) {
      Serial.println("Erro: Falha ao montar o sistema de arquivos SPIFFS.");
      return;
  }

  Serial.println("Setup concluído.");
}

void loop()
{

  if (millis() - lastFetchTime >= FETCH_INTERVAL_MS) {
    lastFetchTime = millis();
    fetchConfigurationFromFirebase(); // Obtem as configuracoes e ja atualiza na memoria Flash do ESP32
    alarmTime = readConfigurationData(); // Atualiza a struct configData com os dados lidos da memoria Flash
    scheduleAlarm(alarmTime.year, alarmTime.month, alarmTime.day, alarmTime.hour, alarmTime.minute, alarmTime.second, alarmTime.cycle, alarmTime.duration); // Agenda o alarme com os dados lidos
  }
  
  //if (millis() - lastPrintTime >=  PRINT_INTERVAL_MS) {
  //  lastPrintTime = millis();
  //  loadAndPrintConfiguration();
  //}
  
  //if (millis() - lastSendTime >=  SEND_INTERVAL_MS) {
  //  lastSendTime = millis();
  //  readFlowRateSensor(flowData);
  //  logSensorDataToFirebase(DHTtemp, flowData[0], flowData[1]);
  //}

 
  imprimirDataHora(myRTC.now());
  if (alarmFiredFlag)
  {
    RTCtemp = myRTC.getTemperature();

    if (myRTC.alarmFired(1))
    {
      myRTC.clearAlarm(1);
      Serial.println(" - Alarm 1 cleared");
      setValveState(true); // Abre a válvula
      printDateTime(myRTC.getAlarm2(), myRTC.getAlarm2Mode());
    }
    else if (myRTC.alarmFired(2))
    {
      dataReadyToSend = true;
      myRTC.clearAlarm(2);
      Serial.println(" - Alarm 2 cleared");
      readFlowRateSensor(flowData);
      DHTtemp =  readDHTSensor();
      RTCtemp = myRTC.getTemperature();
      avg_temp = (DHTtemp + RTCtemp) / 2.0;
      
      setValveState(false); // Fecha a válvula
    }
    alarmFiredFlag = false;
  }
  if (dataReadyToSend) {
    sendSensorDataToFirebase(avg_temp, flowData[0], flowData[1]);
    dataReadyToSend = false;
}
    delay(5000);
} 



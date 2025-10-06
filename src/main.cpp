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

  if (Firebase.signUp(&config, &auth, "", ""))
    signupOK = true;
  else
    Serial.printf("signUp failed: %s\n", config.signer.signupError.message.c_str());

  config.token_status_callback = tokenStatusCallback;
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);

  // Inicia stream para monitorar alterações no caminho
  if (!Firebase.RTDB.beginStream(&fbdo, watchPath.c_str()))
  {
    Serial.printf("beginStream failed: %s\n", fbdo.errorReason().c_str());
  }
  else
  {
    Firebase.RTDB.setStreamCallback(&fbdo, streamCallback, streamTimeoutCallback);
    Serial.println("Ouvindo mudanças em " + watchPath);
  }

  // RTC
  if (!myRTC.begin())
  {
    Serial.println("RTC não encontrado!");
    Serial.flush();
    while (1)
      delay(10);
  }

  myRTC.adjust(DateTime(F(__DATE__), F(__TIME__)));

  // Se perdeu energia, ajusta a hora atual para a hora da compilação
  if (myRTC.lostPower())
  {
    Serial.println("RTC perdeu a hora. Ajustando...");
    myRTC.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }

  myRTC.disable32K();
  myRTC.clearAlarm(1);
  myRTC.clearAlarm(2);
  myRTC.writeSqwPinMode(DS3231_OFF);
  myRTC.disableAlarm(2);

  Serial.println("Setup concluído.");


  

}

void loop()
{
  // vTaskDelay(pdMS_TO_TICKS(5000));
  //printDateTime(myRTC.now(), myRTC.getAlarm1Mode());
  float temperature = myRTC.getTemperature();
  imprimirDataHora(myRTC.now());
  if (alarmFiredFlag)
  {
    if (myRTC.alarmFired(1))
    {
      myRTC.clearAlarm(1);
      Serial.println(" - Alarm 1 cleared");
      setValveState(true); // Abre a válvula
      printDateTime(myRTC.getAlarm2(), myRTC.getAlarm2Mode());
    }
    else if (myRTC.alarmFired(2))
    {
      myRTC.clearAlarm(2);
      Serial.println(" - Alarm 2 cleared");
      sendTemperatureToFirebase(temperature);
      sendFlowRateToFirebase(flowRate);
      sendTotalMilliLitresToFirebase(totalMilliLitres);
      setValveState(false); // Fecha a válvula
    }
    alarmFiredFlag = false;
  }
  if (flowStateFlag)
  {
    unsigned long currentTime;
    unsigned long elapsedTime;

    digitalWrite(debugLedPin2, !digitalRead(debugLedPin2));
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

      // Temperatura

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
  }
  // vTaskDelay(pdMS_TO_TICKS(5000));
  delay(5000);
}

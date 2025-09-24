#include "tasks.h"
#include "globals.h"
#include "rtc_utils.h"
#include "config.h"
#include "valve.h"

void Task1(void *pvParameters) {
  Serial.println("Task1: Iniciada no Core " + String(xPortGetCoreID()));
  if(!myRTC.setAlarm1(alarm1Time, DS3231_A1_Minute)) Serial.println("Error, alarm wasn't set!");
  else Serial.println("Alarm 1 will happen at specified time");

  printDateTime(myRTC.getAlarm1(), myRTC.getAlarm1Mode());

  while (true) {
    if(alarmFiredFlag){
        if (myRTC.alarmFired(1)) {
          myRTC.clearAlarm(1);
          Serial.println(" - Alarm 1 cleared");
          setValveState(true); // Abre a válvula
          //digitalWrite(debugLedPin, HIGH);
          
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
          setValveState(false); // Fecha a válvula
          //digitalWrite(debugLedPin, LOW);
        }
        alarmFiredFlag = false;
      }
      vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void Task2(void *pvParameters) {
  while (true) {
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
    }}

#include "tasks.h"
#include "globals.h"
#include "rtc_utils.h"
#include "config.h"

void Task1(void *pvParameters) {
  Serial.println("Task1: Iniciada no Core " + String(xPortGetCoreID()));
  if(!myRTC.setAlarm1(alarm1Time, DS3231_A1_Minute)) Serial.println("Error, alarm wasn't set!");
  else Serial.println("Alarm 1 will happen at specified time");

  printDateTime(myRTC.getAlarm1(), myRTC.getAlarm1Mode());

  while (true) {
    if(alarmFiredFlag){
      if (myRTC.alarmFired(1)) {
        myRTC.clearAlarm(1);
        if(!myRTC.setAlarm2(alarm2Time, DS3231_A2_Minute)) Serial.println("Error, alarm wasn't set!");
        else printDateTime(myRTC.getAlarm2(), myRTC.getAlarm2Mode());
        digitalWrite(debugLedPin, HIGH);
      } else if (myRTC.alarmFired(2)) {
        myRTC.clearAlarm(2);
        digitalWrite(debugLedPin, LOW);
      }
      alarmFiredFlag = false;
    }
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}

void Task2(void *pvParameters) {
  while (true) {
    digitalWrite(debugLedPin2, !digitalRead(debugLedPin2));
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
  unsigned long currentTime;
  unsigned long elapsedTime;

  while (true) {
    currentTime = millis();
    elapsedTime = currentTime - oldTime;

    if (elapsedTime > 5000) {
      flowRate = (1000.0 / (elapsedTime)) * pulseCount;
      pulseCount = 0;
      oldTime = currentTime;
      flowMilliLitres = (flowRate / 60) * 1000;
      totalMilliLitres += flowMilliLitres;

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
  while (true) vTaskDelay(pdMS_TO_TICKS(1000));
}

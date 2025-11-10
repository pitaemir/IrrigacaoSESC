#pragma once
#include <Firebase_ESP_Client.h>
#include <esp_task_wdt.h>

#define WDT_TIMEOUT 30  // tempo máximo sem "alimentar" o watchdog (em segundos)


void fetchConfigurationFromFirebase(); // Function to fetch configuration data from Firebase
void sendSensorDataToFirebase(float temperature, float flowRate, long totalML); // Function to send sensor data to Firebase
bool sendWithRetry(const String &path, FirebaseJson &json, int maxRetries, int retryDelayMs); // Function to send data with retry mechanism
void logSensorDataToFirebase(float temperature, float flowRate, long totalML); // Function to log sensor data to Firebase
void resendLocalSensorData();
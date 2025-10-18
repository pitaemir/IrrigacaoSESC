#pragma once
#include <Firebase_ESP_Client.h>

void fetchConfigurationFromFirebase(); // Function to fetch configuration data from Firebase
void sendSensorDataToFirebase(float temperature, float flowRate, long totalML); // Function to send sensor data to Firebase
void logSensorDataToFirebase(float temperature, float flowRate, long totalML); // Function to log sensor data to Firebase
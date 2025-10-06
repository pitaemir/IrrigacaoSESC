#pragma once
#include <Firebase_ESP_Client.h>

void streamCallback(FirebaseStream data);   // Callback function that handles incoming stream data
void streamTimeoutCallback(bool timeout);   // Callback function that handles stream timeout events
void sendTemperatureToFirebase(float temperature); // Function to send temperature data to Firebase
void sendFlowRateToFirebase(float flowRate); // Function to send flow rate data to Firebase
void sendTotalMilliLitresToFirebase(unsigned long totalMilliLitres); // Function to send

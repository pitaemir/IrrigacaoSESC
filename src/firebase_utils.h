#pragma once
#include <Firebase_ESP_Client.h>

void streamCallback(FirebaseStream data);   // Callback function that handles incoming stream data
void streamTimeoutCallback(bool timeout);   // Callback function that handles stream timeout events

#include <Arduino.h>
#include <WiFi.h>
#include <Firebase_ESP_Client.h>
#include "addons/TokenHelper.h"
#include "addons/RTDBHelper.h"

// Wifi credentials
#define WIFI_SSID "JR"
#define WIFI_PASSWORD "22070000"

// Firebase project API Key
#define API_KEY "AIzaSyA1LI1y7m9T-av1WW1sDQMSdqyaG6RtDFU"
#define DATABASE_URL "https://hortasesc-9b067-default-rtdb.firebaseio.com/"

FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

bool signupOK = false;

// Variable to track
String watchPath = "/test/control";

// Callback called when the tracked variable changes
void streamCallback(FirebaseStream data) {
  Serial.println("=== STREAM EVENT ===");
  Serial.printf("Path: %s\n", data.streamPath().c_str());
  Serial.printf("Type: %s\n", data.dataType().c_str());
  Serial.printf("Data: %s\n", data.stringData().c_str());

  // true = on
  // false = off
  if (data.dataTypeEnum() == fb_esp_rtdb_data_type_boolean) {
    bool val = data.boolData();
    if (val) {
      Serial.println(">>> CONTROL = true (ON)");
    } else {
      Serial.println(">>> CONTROL = false (OFF)");
    }
}
  Serial.println("====================");
}

void streamTimeoutCallback(bool timeout) {
  if (timeout) {
    Serial.println("Stream timeout, reconectando...");
  }
}

void setup() {
  Serial.begin(115200);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to WIFI");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());

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
}

void loop() {
  // nada especial aqui, o callback é chamado automaticamente
}

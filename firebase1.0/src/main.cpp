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

bool singupOK = false;
unsigned long sendDataPrevMillis = 0;
int test = 0;
String message = "Hello World!";


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
  Serial.println();

  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;

  // corrigido: signUp
  if (Firebase.signUp(&config, &auth, "", "")) {
    Serial.println("signUp OK");
   singupOK = true;
  } else {
    // corrigido: signupError + printf
    Serial.printf("signUp failed: %s\n", config.signer.signupError.message.c_str());
  }

  // corrigido: tokenStatusCallback
  config.token_status_callback = tokenStatusCallback;

  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
}

void loop() {
  if (Firebase.ready() && singupOK &&(millis() - sendDataPrevMillis > 5000 || sendDataPrevMillis == 0)) {
    sendDataPrevMillis = millis();
    Serial.println(test);
    delay(1000); //to remove
    if (Firebase.RTDB.setInt(&fbdo, "test/test_number", test)) {
      Serial.println();
      Serial.println("PASSED");
      Serial.println(test);
      Serial.println("saved to: " + fbdo.dataPath());
      test++;
    } else {
      Serial.println();
      Serial.println("FAILED" + fbdo.errorReason());
    }
      if (Firebase.RTDB.setString(&fbdo, "test/test_message", message)) {
      Serial.println();
      Serial.println("PASSED");
      Serial.println(message);
      Serial.println("saved to: " + fbdo.dataPath());
      test++;
    } else {
      Serial.println();
      Serial.println("FAILED" + fbdo.errorReason());
    }
}
}
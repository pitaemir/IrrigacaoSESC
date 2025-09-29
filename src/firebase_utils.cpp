#include "firebase_utils.h"
#include "globals.h"
#include "rtc_utils.h"
#include "valve.h"
#include "config.h"

void streamCallback(FirebaseStream data)
{
    Serial.println("=== STREAM EVENT ===");
    Serial.printf("Stream Path: %s\n", data.streamPath().c_str());
    Serial.printf("Type: %s\n", data.dataType().c_str());
    Serial.printf("Data: %s\n", data.stringData().c_str());

    if (data.dataTypeEnum() == fb_esp_rtdb_data_type_json)
    {
        FirebaseJson *json = data.to<FirebaseJson *>();
        FirebaseJsonData result;

        if (json->get(result, "year"))
            fbYear = result.intValue;
        if (json->get(result, "month"))
            fbMonth = result.intValue;
        if (json->get(result, "day"))
            fbDay = result.intValue;
        if (json->get(result, "hour"))
            fbHour = result.intValue;
        if (json->get(result, "minute"))
            fbMinute = result.intValue;
        if (json->get(result, "second"))
            fbSecond = result.intValue;

        scheduleAlarm(fbYear, fbMonth, fbDay, fbHour, fbMinute, fbSecond); // Chama o vetor para agendar o alarme
    }
    else if (data.dataTypeEnum() == fb_esp_rtdb_data_type_boolean)
    {
        bool val = data.boolData();
        setValveState(val);
    }
    else if (data.dataTypeEnum() == fb_esp_rtdb_data_type_integer)
    {
        int val = data.intData();
        String path = data.dataPath();
        Serial.println(">>> Recebido um valor inteiro!");
        // Tratamento especifico para o caso de um valor inteiro
        if (path == "/test")
        {
            Serial.println(">>> Alerta: O valor '" + String(val) + "' foi recebido diretamente no caminho '/test'.");
            Serial.println(">>> Isso pode ter sobrescrito todo o objeto JSON, apagando as chaves internas (year, month, etc.).");
            return; // Interrompe a função para evitar processamento incorreto
        }

        String subKey = path.substring(path.lastIndexOf('/') + 1);
        Serial.println(">>> Sub-chave identificada: '" + subKey + "'");

        if (subKey == "year")
        {
            Serial.println(">>> Chave 'year' atualizada para: " + String(val));
            fbYear = val;
            // int year = val;
        }
        else if (subKey == "month")
        {
            Serial.println(">>> Chave 'month' atualizada para: " + String(val));
            fbMonth = val;
            // int month = val;
        }
        else if (subKey == "day")
        {
            Serial.println(">>> Chave 'day' atualizada para: " + String(val));
            fbDay = val;
            // int day = val;
        }
        else if (subKey == "hour")
        {
            Serial.println(">>> Chave 'hour' atualizada para: " + String(val));
            fbHour = val;
            // int hour = val;
        }
        else if (subKey == "minute")
        {
            Serial.println(">>> Chave 'minute' atualizada para: " + String(val));
            fbMinute = val;
            // int minute = val;
        }
        else if (subKey == "second")
        {
            Serial.println(">>> Chave 'second' atualizada para: " + String(val));
            fbSecond = val;
            // int second = val;
        }
        else
        {
            Serial.println(">>> Chave desconhecida '" + subKey + "' com valor: " + String(val));
        }

        scheduleAlarm(fbYear, fbMonth, fbDay, fbHour, fbMinute, fbSecond);
    }

    Serial.println("====================");
}

void streamTimeoutCallback(bool timeout)
{
    if (timeout)
        Serial.println("Stream timeout, reconectando...");
}

void sendSensorDataToFirebase(float flowRate, unsigned long totalMilliLitres) {
    DateTime now = myRTC.now();
  
    FirebaseJson json;
    json.set("flowRate", flowRate);
    json.set("totalMillilitres", totalMilliLitres);
    json.set("timestamp", now.unixtime()); // <<< Gera timestamp em segundos
  
    // Opcional: salva data/hora como strings separadas
    json.set("year", now.year());
    json.set("month", now.month());
    json.set("day", now.day());
    json.set("hour", now.hour());
    json.set("minute", now.minute());
    json.set("second", now.second());
  
    if (Firebase.RTDB.pushJSON(&fbdo, "/sensores/historico", &json)) {
      Serial.println("Leitura enviada ao Firebase com timestamp!");
    } else {
      Serial.printf("Erro ao enviar: %s\n", fbdo.errorReason().c_str());
    }
  }
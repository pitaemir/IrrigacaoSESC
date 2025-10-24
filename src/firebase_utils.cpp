#include "firebase_utils.h"
#include "globals.h"
#include "rtc_utils.h"
#include "valve.h"
#include "config.h"
#include "dataStoraging.h"

void streamCallback(FirebaseStream data)
{
    Serial.println("=== STREAM EVENT ===");
    Serial.printf("Stream Path: %s\n", data.streamPath().c_str());
    Serial.printf("Type: %s\n", data.dataType().c_str());
    Serial.printf("Data: %s\n", data.stringData().c_str());

    if (data.dataTypeEnum() == fb_esp_rtdb_data_type_json)
    {
        FirebaseJson json = data.to<FirebaseJson>();
        FirebaseJsonData result;

        // Usa o operador '.' para acessar os métodos, pois 'json' é um objeto, não um ponteiro.
        if (json.get(result, "year"))
            fbYear = result.intValue;
        if (json.get(result, "month"))
            fbMonth = result.intValue;
        if (json.get(result, "day"))
            fbDay = result.intValue;
        if (json.get(result, "hour"))
            fbHour = result.intValue;
        if (json.get(result, "minute"))
            fbMinute = result.intValue;
        if (json.get(result, "second"))
            fbSecond = result.intValue;
        if (json.get(result, "cycle"))
            fbCycle = result.intValue;
        if (json.get(result, "duration"))
            fbDuration = result.intValue;

        scheduleAlarm(fbYear, fbMonth, fbDay, fbHour, fbMinute, fbSecond, fbCycle); // Chama o vetor para agendar o alarme
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
        }
        else if (subKey == "month")
        {
            Serial.println(">>> Chave 'month' atualizada para: " + String(val));
            fbMonth = val;
        }
        else if (subKey == "day")
        {
            Serial.println(">>> Chave 'day' atualizada para: " + String(val));
            fbDay = val;
        }
        else if (subKey == "hour")
        {
            Serial.println(">>> Chave 'hour' atualizada para: " + String(val));
            fbHour = val;
        }
        else if (subKey == "minute")
        {
            Serial.println(">>> Chave 'minute' atualizada para: " + String(val));
            fbMinute = val;
        }
        else if (subKey == "second")
        {
            Serial.println(">>> Chave 'second' atualizada para: " + String(val));
            fbSecond = val;
        }
        else if (subKey == "cycle")
        {
            Serial.println(">>> Chave 'cycle' atualizada para: " + String(fbCycle));
            fbCycle = val;
        }
        else
        {
            Serial.println(">>> Chave desconhecida '" + subKey + "' com valor: " + String(val));
        }

        scheduleAlarm(fbYear, fbMonth, fbDay, fbHour, fbMinute, fbSecond, fbCycle); // Chama o vetor para agendar o alarme
        storeConfigurationData(fbYear, fbMonth, fbDay, fbHour, fbMinute, fbSecond, fbCycle); // Salva os dados atualizados no SPIFFS
    }

    Serial.println("====================");
}

void streamTimeoutCallback(bool timeout)
{
    if (timeout)
        Serial.println("Stream timeout, reconectando...");
}

void sendTemperatureToFirebase(float temperature){
    DateTime now = myRTC.now();
    if (Firebase.RTDB.setFloat(&fbdo, "/test2/temperature", temperature)) {
            Serial.println("Temperatura enviada para o Firebase com sucesso:");
            Serial.println("Timestamp: " + String(now.unixtime()));
        } else {
            Serial.println("Erro ao enviar temperatura para o Firebase:");
            Serial.println(fbdo.errorReason());
        }
        delay(200);
    }

void sendFlowRateToFirebase(float flowRate){
    DateTime now = myRTC.now();
    if (Firebase.RTDB.setFloat(&fbdo, "/test2/flowRate", flowRate)) {
            Serial.println("Vazão enviada para o Firebase com sucesso:");
            Serial.println("Timestamp: " + String(now.unixtime()));
        } else {
            Serial.println("Erro ao enviar vazão para o Firebase:");
            Serial.println(fbdo.errorReason());
        }
        delay(200);
    }
void sendTotalMilliLitresToFirebase(unsigned long totalMilliLitres){
    DateTime now = myRTC.now();
    if (Firebase.RTDB.setInt(&fbdo, "/test2/totalMilliLitres", totalMilliLitres)) {
            Serial.println("Total de mililitros enviados para o Firebase com sucesso:");
            Serial.println("Timestamp: " + String(now.unixtime()));
        } else {
            Serial.println("Erro ao enviar total de mililitros para o Firebase:");
            Serial.println(fbdo.errorReason());
        }
        delay(200);
    }
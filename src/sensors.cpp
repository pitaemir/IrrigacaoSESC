#include "sensors.h"

#define DHTTYPE DHT11 // DHT 22  (AM2302), AM2321

DHT dht(DHTPIN, DHTTYPE);

void pulseCounter()
{
    pulseCount++;
}

float readDHTSensor()
{
    float humidity = dht.readHumidity();
    float temperature = dht.readTemperature();
    Serial.printf("DHT Sensor - Humidity: %.2f %% Temperature: %.2f °C\n", humidity, temperature);
    return temperature;
}

void readFlowRateSensor(float *data)
{
    unsigned long currentTime;
    unsigned long elapsedTime;

    currentTime = millis();
    elapsedTime = currentTime - oldTime;

    if (elapsedTime > 1000)
    {
        // Calcula a vazão
        flowRate = (1000.0 / (elapsedTime)) * pulseCount;

        // Resetar contadores
        pulseCount = 0;
        oldTime = currentTime;

        // Calcula a quantidade de água passada
        flowMilliLitres = (flowRate / 60) * 1000;

        // Adiciona à quantidade total
        totalMilliLitres += flowMilliLitres;

        data[0] = flowMilliLitres;
        data[1] = totalMilliLitres;
        // Serial.printf("Flow Rate Sensor - Flow Rate: %.2f L/min\n", flowRate);
        Serial.printf("Flow Rate Sensor - Flow MilliLitres: %u mL - Total MilliLitres: %lu mL\n", flowMilliLitres, totalMilliLitres);
    }
}
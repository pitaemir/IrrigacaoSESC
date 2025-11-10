#include "firebase_utils.h"
#include "globals.h"
#include "rtc_utils.h"
#include "valve.h"
#include "config.h"
#include "dataStoraging.h"
#include <time.h>
#include <esp_task_wdt.h>


void fetchConfigurationFromFirebase() {
    Serial.println("\nBuscando dados de configuração do Firebase...");

    // 1. Faz a requisição GET para o caminho 'watchPath'
    if (Firebase.RTDB.getJSON(&fbdo, watchPath.c_str())) {
        // Sucesso na requisição!

        // 2. Verifica se o tipo de dado recebido é um JSON
        if (fbdo.dataTypeEnum() == fb_esp_rtdb_data_type_json) {
            FirebaseJson json = fbdo.to<FirebaseJson>();
            FirebaseJsonData result;

            // 3. Extrai os valores do JSON (lógica que você já tinha)
            // Esta parte se torna a única forma de receber os dados, simplificando tudo.
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

            Serial.println("Dados recebidos e processados com sucesso:");
            Serial.printf("Data: %02d/%02d/%d\n", fbDay, fbMonth, fbYear);
            Serial.printf("Hora: %02d:%02d:%02d\n", fbHour, fbMinute, fbSecond);
            Serial.printf("Ciclo: %d, Duração: %d\n", fbCycle, fbDuration);

            
            // 4. Aciona suas lógicas de negócio
            //scheduleAlarm(fbYear, fbMonth, fbDay, fbHour, fbMinute, fbSecond, fbCycle);
            storeConfigurationData(fbYear, fbMonth, fbDay, fbHour, fbMinute, fbSecond, fbCycle, fbDuration); // aqui

        } else {
            Serial.println("Erro: Os dados recebidos não estão no formato JSON.");
            Serial.printf("Tipo recebido: %s\n", fbdo.dataType().c_str());
        }

    } else {
        // Falha na requisição
        Serial.println("Erro ao buscar dados do Firebase.");
        Serial.printf("Razão: %s\n", fbdo.errorReason().c_str());
    }
}

/**
 * @brief Envia dados de sensores para o nó /test2 no Firebase RTDB.
 * @param temperature A temperatura atual a ser enviada.
 * @param flowRate A taxa de fluxo atual a ser enviada.
 * @param totalML O volume total em mililitros a ser enviado.
 */
void sendSensorDataToFirebase(float temperature, float flowRate, long totalML) {
    // 1. Define o caminho exato para onde os dados serão enviados.
    // Neste caso, diretamente para o nó "test2".
    if (WiFi.status() != WL_CONNECTED || !Firebase.ready()) {
        Serial.println("⚠️ Firebase não está pronto. Salvando dados localmente para enviar depois...");
        saveSensorDataLocally(temperature, flowRate, totalML);
        return;
    }
    
    String path = "/test2";

    // 2. Cria o objeto FirebaseJson para montar a carga de dados.
    FirebaseJson json;
    
    // 3. Adiciona os dados ao objeto JSON.
    // As chaves ("temperature", "flowRate", etc.) devem corresponder exatamente
    // às chaves que você espera ver no Firebase.
    // Use setFloat para números com casas decimais.
    json.set("temperature", temperature);
    json.set("flowRate", flowRate);
    json.set("totalMilliLitres", totalML);

    Serial.printf("Enviando dados para Firebase em %s ...\n", path.c_str());
    // Imprime o JSON que será enviado para facilitar a depuração.
    json.toString(Serial, true); // O 'true' formata a saída para ser legível.
    Serial.println();

    // 4. Envia o objeto JSON para o caminho especificado.
    // A função setJSON sobrescreve todos os dados no caminho 'path'.
    if (Firebase.RTDB.setJSON(&fbdo, path.c_str(), &json)) {
        Serial.println("✅ Dados enviados com sucesso!");
        firebaseFailCount = 0;  // 🔄 zera o contador ao enviar com sucesso
    } else {
        firebaseFailCount++;
        Serial.printf("❌ Falha ao enviar dados (tentativa %d). Razão: %s\n",
                      firebaseFailCount, fbdo.errorReason().c_str());
        saveSensorDataLocally(temperature, flowRate, totalML);
    
        // 🔹 Se ultrapassar o limite de falhas, reinicia o ESP
        if (firebaseFailCount >= MAX_FIREBASE_FAILS) {
            Serial.println("🚨 Falhas consecutivas no Firebase. Reiniciando ESP32...");
            delay(2000);
            ESP.restart();
        }
    }
    
}

/**
 * @brief Adiciona um novo registro de leitura de sensores no Firebase RTDB.
 * Usa pushJSON para criar uma entrada única em um nó de histórico.
 * @param temperature A temperatura atual a ser enviada.
 * @param flowRate A taxa de fluxo atual a ser enviada.
 * @param totalML O volume total em mililitros a ser enviado.
 */
void logSensorDataToFirebase(float temperature, float flowRate, long totalML) {
    // 1. Define o caminho do NÓ PAI onde a lista de leituras será armazenada.
    String path = "/historico_sensores";

    // 2. Cria o objeto JSON para a nova leitura.
    FirebaseJson json;
    
    // 3. Obtém o timestamp atual.
    // time(nullptr) retorna o número de segundos desde 01/01/1970 (Unix Timestamp).
    float now = 17;
    
    // 4. Adiciona todos os dados ao objeto, incluindo o timestamp.
    json.set("timestamp", now);
    json.set("temperature", temperature);
    json.set("flowRate", flowRate);
    json.set("totalMilliLitres", totalML);

    Serial.printf("Adicionando novo registro em %s ...\n", path.c_str());

    // 5. Usa pushJSON para adicionar o novo registro ao Firebase.
    // A biblioteca cuidará de gerar o ID único.
// 4. Envia o objeto JSON para o caminho especificado.
if (sendWithRetry(path, json, 3, 3000)) {
    Serial.println("Envio finalizado com sucesso após retries (se necessários).");
} else {
    Serial.println("❌ Falha permanente: dados não foram enviados ao Firebase.");
    Serial.println("📦 Salvando localmente para envio posterior...");
    saveSensorDataLocally(temperature, flowRate, totalML);
    // ❌ NÃO chamar resendLocalSensorData() aqui — será feito periodicamente no loop()
}

}

bool sendWithRetry(const String &path, FirebaseJson &json, int maxRetries = 3, int retryDelayMs = 2000) {
    for (int attempt = 1; attempt <= maxRetries; attempt++) {
        Serial.printf("Tentativa %d de envio para %s...\n", attempt, path.c_str());

        if (Firebase.RTDB.setJSON(&fbdo, path.c_str(), &json)) {
            Serial.println(">>> Dados enviados com sucesso!");
            return true;  // Sai da função se deu certo
        } else {
            Serial.printf(">>> Erro ao enviar dados. Tentativa %d falhou.\n", attempt);
            Serial.printf("RAZÃO: %s\n", fbdo.errorReason().c_str());
        }

        // Espera antes de tentar novamente (só se não for a última tentativa)
        if (attempt < maxRetries) {
            Serial.printf("Aguardando %d ms antes de tentar novamente...\n", retryDelayMs);
            delay(retryDelayMs);
        }
    }

    // Se chegou aqui, todas as tentativas falharam
    Serial.println("❌ Falha ao enviar dados após múltiplas tentativas.");
    return false;
}

void resendLocalSensorData() {
    if (WiFi.status() != WL_CONNECTED || !Firebase.ready()) {
        Serial.println("⚠️ Sem conexão ou Firebase não pronto. Abortando reenvio local.");
        return;
    }
    if (!SPIFFS.exists(PENDING_FILE)) return;

    File file = SPIFFS.open(PENDING_FILE, FILE_READ);
    if (!file) {
        Serial.println("❌ Erro ao abrir arquivo de dados pendentes.");
        return;
    }

    Serial.println("📤 Reenviando dados pendentes ao Firebase...");

    while (file.available()) {
        String line = file.readStringUntil('\n');
        if (line.length() == 0) continue;

        FirebaseJson json;
        json.setJsonData(line);

        if (Firebase.RTDB.pushJSON(&fbdo, "/historico_sensores", &json)) {
            Serial.println("✅ Registro reenviado com sucesso.");
        } else {
            Serial.printf("⚠️ Falha ao reenviar: %s\n", fbdo.errorReason().c_str());
            break; // sai do loop para tentar depois
        }

        delay(50); // evita congestionamento de rede
    }

    file.close();

    // Se chegou até aqui, todos foram enviados — podemos apagar
    SPIFFS.remove(PENDING_FILE);
    Serial.println("🧹 Dados locais apagados após envio.");
}



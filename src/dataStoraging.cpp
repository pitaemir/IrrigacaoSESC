#include "dataStoraging.h"

const char* CONFIG_FILE = "/config.json";

/**
 * @brief Armazena os dados de configuração em um arquivo JSON no sistema de arquivos SPIFFS.
 * * @param fbYear O ano a ser salvo.
 * @param fbMonth O mês a ser salvo.
 * @param fbDay O dia a ser salvo.
 * @param fbHour A hora a ser salva.
 * @param fbMinute O minuto a ser salvo.
 * @param fbSecond O segundo a ser salvo.
 * @param fbCycle O ciclo a ser salvo.
 * @return true se os dados foram salvos com sucesso, false caso contrário.
 */

bool storeConfigurationData(int fbYear, int fbMonth, int fbDay, int fbHour, int fbMinute, int fbSecond, int fbCycle, int fbDuration){
    // Implementação da função para armazenar os dados de configuração no SPIFFS
    // Retorna true se o armazenamento for bem-sucedido, false caso contrário

    // Exemplo de implementação (substitua pelo código real de armazenamento):
    Serial.println("Armazenando dados de configuração:");
/*     Serial.print("Year: "); Serial.println(fbYear);
    Serial.print("Month: "); Serial.println(fbMonth);
    Serial.print("Day: "); Serial.println(fbDay);
    Serial.print("Hour: "); Serial.println(fbHour);
    Serial.print("Minute: "); Serial.println(fbMinute);
    Serial.print("Second: "); Serial.println(fbSecond);
    Serial.print("Cycle: "); Serial.println(fbCycle); */
    Serial.printf("Data: %02d/%02d/%d\n", fbDay, fbMonth, fbYear);
    Serial.printf("Hora: %02d:%02d:%02d\n", fbHour, fbMinute, fbSecond);
    Serial.printf("Ciclo: %d, Duração: %d\n", fbCycle, fbDuration);

    // Aqui você adicionaria o código para realmente salvar os dados no SPIFFS
    // 1. Criar um objeto JSON para armazenar os dados.
    // O tamanho (256 bytes) é uma estimativa segura para esses dados.
    StaticJsonDocument<256> doc;

    // Adiciona os valores ao documento JSON com chaves descritivas.
    doc["year"]   = fbYear;
    doc["month"]  = fbMonth;
    doc["day"]    = fbDay;
    doc["hour"]   = fbHour;
    doc["minute"] = fbMinute;
    doc["second"] = fbSecond;
    doc["cycle"]  = fbCycle;
    doc["duration"] = fbDuration; // se quiser adicionar isso ao JSON futuramente

    // 2. Abrir o arquivo no SPIFFS em modo de escrita ('w').
    // Se o arquivo não existir, ele será criado. Se existir, será sobrescrito.
    File configFile = SPIFFS.open(CONFIG_FILE, "w");
    if (!configFile) {
        Serial.println("Erro: Falha ao abrir o arquivo de configuração para escrita.");
        return false;
    }

    // 3. Serializar o JSON para o arquivo.
    // A função serializeJson() converte o objeto 'doc' em uma string JSON e a escreve no arquivo.
    if (serializeJson(doc, configFile) == 0) {
        Serial.println("Erro: Falha ao escrever no arquivo de configuração.");
        configFile.close();
        return false;
    }

    // 4. Fechar o arquivo para garantir que os dados sejam gravados.
    configFile.close();

    Serial.println("Dados de configuração salvos com sucesso.");
    return true;
};


/**
 * @brief Função auxiliar para carregar e imprimir o arquivo de configuração.
 * Útil para verificar se os dados foram salvos corretamente.
 */
void loadAndPrintConfiguration() {
    File configFile = SPIFFS.open(CONFIG_FILE, "r");
    if (!configFile) {
        Serial.println("Arquivo de configuração não encontrado.");
        return;
    }

    Serial.println("\n--- Conteúdo do Arquivo de Configuração ---");
    while (configFile.available()) {
        Serial.write(configFile.read());
    }
    Serial.println("\n------------------------------------------\n");
    configFile.close();
}


/**
 * @brief Lê o arquivo de configuração armazenado em SPIFFS e retorna um struct com os valores.
 * @return ConfigData contendo os valores lidos. Caso haja erro, retorna com -1 em todos os campos.
 */
ConfigData readConfigurationData() {
    ConfigData config = {-1, -1, -1, -1, -1, -1, -1, -1};

    if (!SPIFFS.begin(true)) {
        Serial.println("Erro ao montar SPIFFS.");
        return config;
    }

    File configFile = SPIFFS.open(CONFIG_FILE, "r");
    if (!configFile) {
        Serial.println("Erro: Arquivo de configuração não encontrado.");
        return config;
    }

    StaticJsonDocument<256> doc;
    DeserializationError error = deserializeJson(doc, configFile);
    configFile.close();

    if (error) {
        Serial.print("Erro ao ler JSON: ");
        Serial.println(error.c_str());
        return config;
    }

    // Atribui os valores lidos
    config.year     = doc["year"]   | -1;
    config.month    = doc["month"]  | -1;
    config.day      = doc["day"]    | -1;
    config.hour     = doc["hour"]   | -1;
    config.minute   = doc["minute"] | -1;
    config.second   = doc["second"] | -1;
    config.cycle    = doc["cycle"]  | -1;
    config.duration = doc["duration"] | -1;  // se quiser adicionar isso ao JSON futuramente

    Serial.println("Configuração carregada com sucesso!");
    Serial.printf("Data: %02d/%02d/%04d\n", config.day, config.month, config.year);
    Serial.printf("Hora: %02d:%02d:%02d\n", config.hour, config.minute, config.second);
    Serial.printf("Ciclo: %d, Duração: %d\n", config.cycle, config.duration);

    return config;
}

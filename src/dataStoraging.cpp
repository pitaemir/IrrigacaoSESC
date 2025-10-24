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

bool storeConfigurationData(int fbYear, int fbMonth, int fbDay, int fbHour, int fbMinute, int fbSecond, int fbCycle){
    // Implementação da função para armazenar os dados de configuração no SPIFFS
    // Retorna true se o armazenamento for bem-sucedido, false caso contrário

    // Exemplo de implementação (substitua pelo código real de armazenamento):
    Serial.println("Armazenando dados de configuração:");
    Serial.print("Year: "); Serial.println(fbYear);
    Serial.print("Month: "); Serial.println(fbMonth);
    Serial.print("Day: "); Serial.println(fbDay);
    Serial.print("Hour: "); Serial.println(fbHour);
    Serial.print("Minute: "); Serial.println(fbMinute);
    Serial.print("Second: "); Serial.println(fbSecond);
    Serial.print("Cycle: "); Serial.println(fbCycle);

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
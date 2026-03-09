#include "ConfiguracaoPersistente.h"

// Construtor: Apenas chama a inicialização do SPIFFS
ConfiguracaoPersistente::ConfiguracaoPersistente() {
    initSPIFFS();
}

/**
 * @brief Inicializa o sistema de arquivos SPIFFS.
 * @return true se a inicialização foi bem-sucedida, false caso contrário.
 */
bool ConfiguracaoPersistente::initSPIFFS() {
    Serial.println("Inicializando SPIFFS...");
    if (!SPIFFS.begin(true)) { // O 'true' formata a flash se falhar
        Serial.println("FATAL: Falha ao montar o SPIFFS.");
        return false;
    }
    Serial.println("SPIFFS montado com sucesso.");
    return true;
}

/**
 * @brief Carrega os dados de configuração da Flash.
 * @return true se o arquivo foi lido e o JSON foi deserializado com sucesso.
 */
bool ConfiguracaoPersistente::carregar() {
    if (!SPIFFS.exists(ARQUIVO_CONFIG)) {
        Serial.println("Arquivo de configuracao nao encontrado. Usando padroes.");
        return false;
    }

    File file = SPIFFS.open(ARQUIVO_CONFIG, "r");
    if (!file) {
        Serial.println("ERRO: Falha ao abrir arquivo para leitura.");
        return false;
    }

    // Aloca memória estática para o documento JSON
    StaticJsonDocument<JSON_BUFFER_SIZE> doc;

    // Deserializa o JSON
    DeserializationError error = deserializeJson(doc, file);
    file.close();

    if (error) {
        Serial.print("ERRO: Falha ao deserializar JSON: ");
        Serial.println(error.c_str());
        return false;
    }

    // Carrega os dados da memória (JSON) para os atributos da classe
    dia = doc["dia"] | 0; // O | 0 é o valor padrão caso o campo não exista
    mes = doc["mes"] | 0;
    ano = doc["ano"] | 0;
    hora = doc["hora"] | 0;
    minuto = doc["minuto"] | 0;
    segundo = doc["segundo"] | 0;
    duracao = doc["duracao"] | 0;
    ciclo = doc["ciclo"].as<String>();

    Serial.println("Configuracao carregada da Flash.");
    return true;
}

/**
 * @brief Salva os dados de configuração atuais na Flash.
 * @return true se o arquivo foi escrito com sucesso.
 */
bool ConfiguracaoPersistente::salvar() {
    // Aloca memória estática para o documento JSON
    StaticJsonDocument<JSON_BUFFER_SIZE> doc;

    // Popula o documento JSON com os atributos atuais da classe
    doc["dia"] = dia;
    doc["mes"] = mes;
    doc["ano"] = ano;
    doc["hora"] = hora;
    doc["minuto"] = minuto;
    doc["segundo"] = segundo;
    doc["duracao"] = duracao;
    doc["ciclo"] = ciclo;

    File file = SPIFFS.open(ARQUIVO_CONFIG, "w");
    if (!file) {
        Serial.println("ERRO: Falha ao abrir arquivo para escrita.");
        return false;
    }

    // Serializa (escreve) o JSON no arquivo
    if (serializeJson(doc, file) == 0) {
        Serial.println("ERRO: Falha ao serializar JSON para escrita.");
        file.close();
        return false;
    }

    file.close();
    Serial.println("Configuracao salva na Flash.");
    return true;
}

/**
 * @brief Salva temporariamente os dados recebidos do formulário nos atributos da classe.
 */
void ConfiguracaoPersistente::salvarTemporariamente(int d, int m, int a, int h, int min, int s, int dur, String c) {
    dia = d;
    mes = m;
    ano = a;
    hora = h;
    minuto = min;
    segundo = s;
    duracao = dur;
    ciclo = c;
    // O método 'salvar()' deve ser chamado separadamente para persistir na Flash!
    _atualizada = true;
}

/**
 * @brief Imprime a configuração salva no Monitor Serial.
 */
void ConfiguracaoPersistente::imprimir() {
    Serial.println("----------------------------------------------");
    Serial.println("Configuracao de Acionamento Atual (Memoria):");
    if (ano != 0) {
        Serial.print("Data Programada: "); Serial.print(dia); Serial.print("/"); 
        Serial.print(mes); Serial.print("/"); Serial.println(ano);
        Serial.print("Hora Programada: "); Serial.print(hora); Serial.print(":"); 
        Serial.print(minuto); Serial.print(":"); Serial.println(segundo);
        Serial.print("Duracao: "); Serial.print(duracao); Serial.println(" minutos");
        Serial.print("Ciclo: "); Serial.println(ciclo);
    } else {
        Serial.println("Config padrao / Vazia.");
    }
    Serial.println("----------------------------------------------");
}
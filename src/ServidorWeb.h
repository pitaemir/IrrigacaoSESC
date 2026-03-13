#ifndef SERVIDORWEB_H
#define SERVIDORWEB_H

#include <WiFi.h>
#include "ConfiguracaoPersistente.h"
#include "Rele.h"

/**
 * @brief Classe para gerenciar a interface web, Wi-Fi e processamento de requisições.
 */
class ServidorWeb {
private:
    const char* ssid;
    const char* password;
    WiFiServer server;
    String header;

    // Métodos privados
    String getParameterValue(String uri, String param);
    String getRequestPath(String requestLine);
    String decodeURL(String text);

    void gerarPaginaHTML(
        WiFiClient client,
        String valvulaEstado,
        float temperatura,
        float umidade,
        float fluxoAtual,
        float fluxoTotal,
        String horarioAtual,
        String dataAtual
    );

    void gerarPaginaConfiguracao(WiFiClient client);

public:
    // Construtor
    ServidorWeb(const char* s, const char* p);

    // Métodos públicos
    void iniciarAP();

    void manusearClientes(
        Rele& valvula,
        ConfiguracaoPersistente& config,
        float temperatura,
        float umidade,
        float fluxoAtual,
        float fluxoTotal,
        String horarioAtual,
        String dataAtual
    );
};

#endif // SERVIDORWEB_H
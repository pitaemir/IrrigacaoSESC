#ifndef SERVIDORWEB_H
#define SERVIDORWEB_H

#include <WiFi.h>
#include "ConfiguracaoPersistente.h"
#include "Rele.h"

class ServidorWeb {
private:
    const char* ssid;
    const char* password;
    ConfiguracaoPersistente* configAtual;
    WiFiServer server;
    String header;

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
    ServidorWeb(const char* s, const char* p, ConfiguracaoPersistente* config);

    void iniciarAP();
    int getNumeroAtivacao() const { return configAtual->getNumeroAtivacao(); }

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

#endif
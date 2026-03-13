#include "ServidorWeb.h"
#include <Arduino.h>

// =========================================================
// CONSTRUTOR
// =========================================================
ServidorWeb::ServidorWeb(const char* s, const char* p)
    : ssid(s), password(p), server(80) {}


// =========================================================
// INICIAR ACCESS POINT
// =========================================================
void ServidorWeb::iniciarAP() {
    Serial.print("Configurando Ponto de Acesso...");
    WiFi.softAP(ssid, password);
    Serial.println("Pronto.");

    Serial.print("Endereço IP do AP: ");
    Serial.println(WiFi.softAPIP());

    server.begin();
    Serial.println("Servidor web iniciado.");
}


// =========================================================
// EXTRAI PARAMETRO DA URL
// =========================================================
String ServidorWeb::getParameterValue(String uri, String param) {
    String searchStr = param + "=";
    int startIndex = uri.indexOf(searchStr);

    if (startIndex < 0) return "";

    startIndex += searchStr.length();
    int endIndex = uri.indexOf('&', startIndex);
    if (endIndex < 0) endIndex = uri.indexOf(' ', startIndex);

    return uri.substring(startIndex, endIndex);
}


// =========================================================
// FUNÇÃO PRINCIPAL DO SERVIDOR WEB
// =========================================================
void ServidorWeb::manusearClientes(
    Rele& valvula,
    ConfiguracaoPersistente& config,
    float temperatura,
    float umidade,
    float fluxoAtual,
    float fluxoTotal,
    String horarioAtual,
    String dataAtual
) {
    WiFiClient client = server.available();

    if (!client) return;

    Serial.println("Novo cliente conectado.");
    String currentLine = "";
    header = "";

    while (client.connected()) {
        if (client.available()) {
            char c = client.read();
            header += c;

            if (c == '\n') {
                if (currentLine.length() == 0) {

                    // =========================================================
                    // CONTROLE DA VÁLVULA
                    // =========================================================
                    if (header.indexOf("GET /valvula/on") >= 0) {
                        valvula.ligar();
                        client.println("HTTP/1.1 303 See Other");
                        client.println("Location: /");
                        client.println();
                        break;
                    }

                    if (header.indexOf("GET /valvula/off") >= 0) {
                        valvula.desligar();
                        client.println("HTTP/1.1 303 See Other");
                        client.println("Location: /");
                        client.println();
                        break;
                    }

                    // =========================================================
                    // PARAR CICLO DE IRRIGAÇÃO
                    // =========================================================
                    if (header.indexOf("GET /parar") >= 0) {
                        valvula.desligar();
                        config.solicitarCancelamentoCiclo();

                        client.println("HTTP/1.1 303 See Other");
                        client.println("Location: /");
                        client.println();
                        break;
                    }

                    // =========================================================
                    // SALVAR CONFIGURAÇÕES
                    // =========================================================
                    if (header.indexOf("GET /save") >= 0) {

                        int startUri = header.indexOf("GET /save");
                        int endUri = header.indexOf(" HTTP/1.1");
                        String uri = header.substring(startUri + 4, endUri);

                        config.salvarTemporariamente(
                            getParameterValue(uri, "dia").toInt(),
                            getParameterValue(uri, "mes").toInt(),
                            getParameterValue(uri, "ano").toInt(),
                            getParameterValue(uri, "hora").toInt(),
                            getParameterValue(uri, "minuto").toInt(),
                            getParameterValue(uri, "segundo").toInt(),
                            getParameterValue(uri, "duracao").toInt(),
                            getParameterValue(uri, "ciclo").toInt()
                        );

                        config.salvar();

                        client.println("HTTP/1.1 303 See Other");
                        client.println("Location: /");
                        client.println();
                        break;
                    }

                    // =========================================================
                    // PÁGINA PRINCIPAL
                    // =========================================================
                    String estado = valvula.estaLigado() ? "on" : "off";

                    gerarPaginaHTML(
                        client,
                        estado,
                        temperatura,
                        umidade,
                        fluxoAtual,
                        fluxoTotal,
                        horarioAtual,
                        dataAtual
                    );

                    client.println();
                    break;
                } else {
                    currentLine = "";
                }
            } else if (c != '\r') {
                currentLine += c;
            }
        }
    }

    client.stop();
    Serial.println("Cliente desconectado.");
}


// =========================================================
// GERAR HTML COMPLETO
// =========================================================
void ServidorWeb::gerarPaginaHTML(
    WiFiClient client,
    String valvulaEstado,
    float temperatura,
    float umidade,
    float fluxoAtual,
    float fluxoTotal,
    String horarioAtual,
    String dataAtual
) {
    client.println("HTTP/1.1 200 OK");
    client.println("Content-type:text/html");
    client.println("Connection: close");
    client.println(); 
    
    client.println("<!DOCTYPE html><html lang=\"pt-br\">");
    client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
    client.println("<meta charset=\"UTF-8\">");
    client.println("<title>Controle da Horta</title>");
    
    // CSS
    client.println("<style>");
    client.println("body { font-family: Arial; background-color: #f4f7f6; color: #333; margin: 0; padding: 0; }");
    client.println(".container { max-width: 600px; margin: 20px auto; padding: 20px; }");
    client.println(".status-box, .card { background: white; border-radius: 12px; padding: 20px; margin-top: 20px; box-shadow: 0 4px 12px rgba(0,0,0,0.05); }");
    client.println(".status-text-on { color: #28a745; font-weight: bold; }");
    client.println(".status-text-off { color: #dc3545; font-weight: bold; }");

    /* novos estilos do formulário */
    client.println("form { display: flex; flex-direction: column; gap: 12px; }");
    client.println(".form-group { display: flex; flex-direction: column; }");
    client.println("label { font-weight: bold; margin-bottom: 4px; }");
    client.println("input, select { padding: 10px; border: 1px solid #ccc; border-radius: 8px; font-size: 14px; width: 100%; box-sizing: border-box; }");
    client.println("button { margin-top: 10px; padding: 12px; border: none; border-radius: 8px; background-color: #28a745; color: white; font-size: 16px; cursor: pointer; }");
    client.println("button:hover { background-color: #218838; }");
    client.println("</style>");
    
    client.println("</head><body><div class='container'>");
    
    // === Título ===
    client.println("<h1>Controle Remoto da Horta</h1>");

    // === STATUS DA VÁLVULA ===
    client.println("<div class='status-box'>");
    if (valvulaEstado == "off") {
        client.println("<p>Status da válvula: <span class='status-text-off'>Desligada</span></p>");
        client.println("<p><a href='/valvula/on'>Ligar</a></p>");
    } else {
        client.println("<p>Status da válvula: <span class='status-text-on'>Ligada</span></p>");
        client.println("<p><a href='/valvula/off'>Desligar</a></p>");
    }
    client.println("</div>");

    // === DATA E HORA ===
    client.println("<div class='card'>");
    client.println("<h2>Data e Hora</h2>");
    client.printf("<p><b>Data:</b> %s</p>", dataAtual.c_str());
    client.printf("<p><b>Hora:</b> %s</p>", horarioAtual.c_str());
    client.println("</div>");

    // === SENSORES ===
    client.println("<div class='card'>");
    client.println("<h2>Sensores</h2>");
    client.printf("<p><b>Temperatura:</b> %.1f °C</p>", temperatura);
    client.printf("<p><b>Umidade:</b> %.1f %%</p>", umidade);
    client.printf("<p><b>Fluxo Atual:</b> %.2f L/min</p>", fluxoAtual);
    client.printf("<p><b>Total Irrigado:</b> %.2f L</p>", fluxoTotal);
    client.println("</div>");

    // === FORMULÁRIO ANTIGO — SEM ALTERAÇÕES ===
    client.println("<div class='card'>");
    client.println("<h2>Programar Irrigação</h2>");
    client.println("<form action='/save' method='GET'>");

    client.println("<div class='form-group'>");
    client.println("<label for='dia'>Dia:</label>");
    client.println("<input type='text' id='dia' name='dia'>");
    client.println("</div>");

    client.println("<div class='form-group'>");
    client.println("<label for='mes'>Mês:</label>");
    client.println("<input type='text' id='mes' name='mes'>");
    client.println("</div>");

    client.println("<div class='form-group'>");
    client.println("<label for='ano'>Ano:</label>");
    client.println("<input type='text' id='ano' name='ano'>");
    client.println("</div>");

    client.println("<div class='form-group'>");
    client.println("<label for='hora'>Hora:</label>");
    client.println("<input type='text' id='hora' name='hora'>");
    client.println("</div>");

    client.println("<div class='form-group'>");
    client.println("<label for='minuto'>Minuto:</label>");
    client.println("<input type='text' id='minuto' name='minuto'>");
    client.println("</div>");

    client.println("<div class='form-group'>");
    client.println("<label for='segundo'>Segundo:</label>");
    client.println("<input type='text' id='segundo' name='segundo'>");
    client.println("</div>");

    client.println("<div class='form-group'>");
    client.println("<label for='duracao'>Duração (min):</label>");
    client.println("<input type='text' id='duracao' name='duracao'>");
    client.println("</div>");

    client.println("<div class='form-group'>");
    client.println("<label for='ciclo'>Ciclo:</label>");
    client.println("<select id='ciclo' name='ciclo'>");
    client.println("<option value='6'>A cada 6 horas</option>");
    client.println("<option value='12'>A cada 12 horas</option>");
    client.println("<option value='24'>A cada 24 horas</option>");
    client.println("<option value='0'>Único</option>");
    client.println("</select>");
    client.println("</div>");

    client.println("<button type='submit'>Salvar</button>");
    client.println("</form></div>");

    client.println("<div class='card'>");
    client.println("<h2>Controle de Ciclo</h2>");
    client.println("<p>Interromper irrigação automática</p>");
    client.println("<a href='/parar'><button style='background-color:#dc3545'>Parar Ciclo</button></a>");
    client.println("</div>");

    client.println("</div></body></html>");
}

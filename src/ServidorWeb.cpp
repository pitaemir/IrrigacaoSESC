#include "ServidorWeb.h"
#include <Arduino.h>
#include <time.h>


static String doisDigitos(const String& valor) {
    return (valor.length() < 2) ? "0" + valor : valor;
}

static String formatarHorario(String horario) {
    int p1 = horario.indexOf(':');
    int p2 = horario.indexOf(':', p1 + 1);

    if (p1 == -1 || p2 == -1) {
        return horario;
    }

    String h = horario.substring(0, p1);
    String m = horario.substring(p1 + 1, p2);
    String s = horario.substring(p2 + 1);

    return doisDigitos(h) + ":" + doisDigitos(m) + ":" + doisDigitos(s);
}
static String formatarDataHora(time_t valor) {
    struct tm* t = localtime(&valor);
    if (!t) return "--";

    char buffer[25];
    snprintf(
        buffer,
        sizeof(buffer),
        "%02d/%02d/%04d %02d:%02d:%02d",
        t->tm_mday,
        t->tm_mon + 1,
        t->tm_year + 1900,
        t->tm_hour,
        t->tm_min,
        t->tm_sec
    );
    return String(buffer);
}

// =========================================================
// CONSTRUTOR
// =========================================================
ServidorWeb::ServidorWeb(const char* s, const char* p, ConfiguracaoPersistente* config)
    : ssid(s), password(p), configAtual(config), server(80) {
}

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
String ServidorWeb::getRequestPath(String requestLine) {
    int firstSpace = requestLine.indexOf(' ');
    int secondSpace = requestLine.indexOf(' ', firstSpace + 1);

    if (firstSpace < 0 || secondSpace < 0) return "/";

    return requestLine.substring(firstSpace + 1, secondSpace);
}
String ServidorWeb::decodeURL(String text) {
    text.replace("%2F", "/");
    text.replace("%3A", ":");
    text.replace("%20", " ");
    text.replace("+", " ");
    return text;
}

bool ServidorWeb::converterParaEpoch(
    const String& data,
    const String& horario,
    time_t& epoch
) {
    int barra1 = data.indexOf('/');
    int barra2 = data.indexOf('/', barra1 + 1);

    int p1 = horario.indexOf(':');
    int p2 = horario.indexOf(':', p1 + 1);

    if (barra1 < 0 || barra2 < 0 || p1 < 0 || p2 < 0) {
        return false;
    }

    int dia = data.substring(0, barra1).toInt();
    int mes = data.substring(barra1 + 1, barra2).toInt();
    int ano = data.substring(barra2 + 1).toInt();

    int hora = horario.substring(0, p1).toInt();
    int minuto = horario.substring(p1 + 1, p2).toInt();
    int segundo = horario.substring(p2 + 1).toInt();

    struct tm t = {};
    t.tm_mday = dia;
    t.tm_mon = mes - 1;
    t.tm_year = ano - 1900;
    t.tm_hour = hora;
    t.tm_min = minuto;
    t.tm_sec = segundo;

    epoch = mktime(&t);
    return (epoch != -1);
}

String ServidorWeb::formatarDuracao(int duracaoMinutos) {
    if (duracaoMinutos <= 0) return "0 min";

    int horas = duracaoMinutos / 60;
    int minutos = duracaoMinutos % 60;

    if (horas > 0 && minutos > 0) {
        return String(horas) + "h " + String(minutos) + "min";
    }

    if (horas > 0) {
        return String(horas) + "h";
    }

    return String(minutos) + " min";
}

String ServidorWeb::calcularProximoDisparo(String dataAtual, String horarioAtual) {
    if (configAtual == nullptr) {
        return "--";
    }

    horarioAtual = formatarHorario(horarioAtual);

    time_t agoraEpoch;
    if (!converterParaEpoch(dataAtual, horarioAtual, agoraEpoch)) {
        return "--";
    }

    int dia = configAtual->getDia();
    int mes = configAtual->getMes();
    int ano = configAtual->getAno();
    int hora = configAtual->getHora();
    int minuto = configAtual->getMinuto();
    int segundo = configAtual->getSegundo();
    int ciclo = configAtual->getCiclo();

    char dataConfig[11];
    char horaConfig[9];

    snprintf(dataConfig, sizeof(dataConfig), "%02d/%02d/%04d", dia, mes, ano);
    snprintf(horaConfig, sizeof(horaConfig), "%02d:%02d:%02d", hora, minuto, segundo);

    time_t disparoEpoch;
    if (!converterParaEpoch(String(dataConfig), String(horaConfig), disparoEpoch)) {
        return "--";
    }

    // Disparo único
    if (ciclo == 0) {
        if (disparoEpoch >= agoraEpoch) {
            return formatarDataHora(disparoEpoch);
        }
        return "Agendamento único já executado";
    }

    // Disparo cíclico
    long intervaloSegundos = (long)ciclo * 3600L;

    while (disparoEpoch < agoraEpoch) {
        disparoEpoch += intervaloSegundos;
    }

    return formatarDataHora(disparoEpoch);
}


// =========================================================
// FUNÇÃO AUXILIAR PARA GERAR PÁGINA DE CONFIGURAÇÃO
// =========================================================
void ServidorWeb::gerarPaginaConfiguracao(WiFiClient client) {
    client.println("HTTP/1.1 200 OK");
    client.println("Content-type:text/html");
    client.println("Connection: close");
    client.println();

    client.println("<!DOCTYPE html><html lang='pt-br'>");
    client.println("<head>");
    client.println("<meta name='viewport' content='width=device-width, initial-scale=1'>");
    client.println("<meta charset='UTF-8'>");
    client.println("<title>Configurar Irrigação</title>");

    client.println("<style>");
    client.println("body { font-family: Arial; background-color: #f4f7f6; color: #333; margin: 0; padding: 0; }");
    client.println(".container { max-width: 600px; margin: 20px auto; padding: 20px; }");
    client.println(".card { background: white; border-radius: 12px; padding: 20px; margin-top: 20px; box-shadow: 0 4px 12px rgba(0,0,0,0.05); }");
    client.println("form { display: flex; flex-direction: column; gap: 12px; }");
    client.println(".form-group { display: flex; flex-direction: column; }");
    client.println("label { font-weight: bold; margin-bottom: 4px; }");
    client.println("input, select { padding: 10px; border: 1px solid #ccc; border-radius: 8px; font-size: 14px; width: 100%; box-sizing: border-box; }");
    client.println("button { margin-top: 10px; padding: 12px; border: none; border-radius: 8px; background-color: #28a745; color: white; font-size: 16px; cursor: pointer; }");
    client.println("a { text-decoration: none; }");
    client.println(".btn-voltar { display: inline-block; margin-top: 12px; padding: 12px; border-radius: 8px; background-color: #6c757d; color: white; }");
    client.println("</style>");

    client.println("</head><body>");
    client.println("<div class='container'>");
    client.println("<h1>Configurar Irrigação</h1>");

    client.println("<div class='card'>");
    client.println("<form action='/save' method='GET'>");

    client.println("<div class='form-group'>");
    client.println("<label for='data'>Data:</label>");
    client.println("<input type='text' id='data' name='data' placeholder='dd/mm/aaaa'>");
    client.println("</div>");

    client.println("<div class='form-group'>");
    client.println("<label for='hora'>Hora:</label>");
    client.println("<input type='number' id='hora' name='hora' min='0' max='23'>");
    client.println("</div>");

    client.println("<div class='form-group'>");
    client.println("<label for='duracao'>Duração (min):</label>");
    client.println("<input type='number' id='duracao' name='duracao' min='1'>");
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
    client.println("</form>");

    client.println("<a class='btn-voltar' href='/'>Voltar</a>");
    client.println("</div>");
    client.println("</div>");
    client.println("</body></html>");
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
    String requestLine = "";
    header = "";

    while (client.connected()) {
        if (client.available()) {
            char c = client.read();
            header += c;

            if (c == '\n') {
                // captura a PRIMEIRA linha da requisição
                if (requestLine.length() == 0) {
                    requestLine = currentLine;
                }

                if (currentLine.length() == 0) {
                    String path = getRequestPath(requestLine);

                    Serial.print("Requisição: ");
                    Serial.println(requestLine);

                    Serial.print("Path detectado: ");
                    Serial.println(path);

                    // ============================
                    // CONTROLE DA VÁLVULA
                    // ============================
                    if (path == "/valvula/on") {
                        valvula.ligar();
                        client.println("HTTP/1.1 303 See Other");
                        client.println("Location: /");
                        client.println();
                        break;
                    }

                    if (path == "/valvula/off") {
                        valvula.desligar();
                        client.println("HTTP/1.1 303 See Other");
                        client.println("Location: /");
                        client.println();
                        break;
                    }

                    // ============================
                    // PARAR CICLO
                    // ============================
                    if (path == "/parar") {
                        valvula.desligar();
                        config.solicitarCancelamentoCiclo();

                        client.println("HTTP/1.1 303 See Other");
                        client.println("Location: /");
                        client.println();
                        break;
                    }

                    // ============================
                    // ABRIR CONFIGURAÇÃO
                    // ============================
                    if (path == "/config") {
                        gerarPaginaConfiguracao(client);
                        client.println();
                        break;
                    }

                    // ============================
                    // SALVAR CONFIGURAÇÕES
                    // ============================
                    if (path.startsWith("/save")) {
                        String data = decodeURL(getParameterValue(path, "data"));
                        int hora = getParameterValue(path, "hora").toInt();
                        int duracao = getParameterValue(path, "duracao").toInt();
                        int ciclo = getParameterValue(path, "ciclo").toInt();

                        int dia = 0;
                        int mes = 0;
                        int ano = 0;

                        int barra1 = data.indexOf('/');
                        int barra2 = data.indexOf('/', barra1 + 1);

                        if (barra1 > 0 && barra2 > barra1) {
                            dia = data.substring(0, barra1).toInt();
                            mes = data.substring(barra1 + 1, barra2).toInt();
                            ano = data.substring(barra2 + 1).toInt();
                        }

                        Serial.println("=== CONFIG SALVA ===");
                        Serial.print("Data: "); Serial.println(data);
                        Serial.print("Dia: "); Serial.println(dia);
                        Serial.print("Mes: "); Serial.println(mes);
                        Serial.print("Ano: "); Serial.println(ano);
                        Serial.print("Hora: "); Serial.println(hora);
                        Serial.print("Duracao: "); Serial.println(duracao);
                        Serial.print("Ciclo: "); Serial.println(ciclo);

                        config.salvarTemporariamente(
                            dia,
                            mes,
                            ano,
                            hora,
                            0,   // minuto fixo
                            0,   // segundo fixo
                            duracao,
                            ciclo
                        );

                        // config.setDia(dia);
                        // config.setMes(mes);
                        // config.setAno(ano);
                        // config.setHora(hora);
                        // config.setDuracao(duracao);
                        // config.setCiclo(ciclo);
                        // config.setAtualizada(true);

                        config.salvar();

                        client.println("HTTP/1.1 303 See Other");
                        client.println("Location: /");
                        client.println();
                        break;
                    }

                    // ============================
                    // PÁGINA PRINCIPAL
                    // ============================
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
// GERAR HTML DA PÁGINA PRINCIPAL
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
    String proximoDisparo = calcularProximoDisparo(dataAtual, horarioAtual);
    String duracaoProgramada = formatarDuracao(configAtual->getDuracao());

    String cicloTexto;
    int ciclo = configAtual->getCiclo();
    if (ciclo == 0) {
        cicloTexto = "Único";
    } else {
        cicloTexto = "A cada " + String(ciclo) + " hora(s)";
    }
    horarioAtual = formatarHorario(horarioAtual);
    client.println("HTTP/1.1 200 OK");
    client.println("Content-type:text/html");
    client.println("Connection: close");
    client.println();

    client.println("<!DOCTYPE html><html lang=\"pt-br\">");
    client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
    client.println("<meta charset=\"UTF-8\">");
    client.println("<title>Controle da Horta</title>");

    client.println("<style>");
    client.println("body { font-family: Arial; background-color: #f4f7f6; color: #333; margin: 0; padding: 0; }");
    client.println(".container { max-width: 600px; margin: 20px auto; padding: 20px; }");
    client.println(".status-box, .card { background: white; border-radius: 12px; padding: 20px; margin-top: 20px; box-shadow: 0 4px 12px rgba(0,0,0,0.05); }");
    client.println(".status-text-on { color: #28a745; font-weight: bold; }");
    client.println(".status-text-off { color: #dc3545; font-weight: bold; }");
    client.println("button { margin-top: 10px; padding: 12px; border: none; border-radius: 8px; color: white; font-size: 16px; cursor: pointer; }");
    client.println(".btn-parar { background-color:#dc3545; }");
    client.println(".btn-config { background-color:#007bff; }");
    client.println("a { text-decoration: none; }");
    client.println("</style>");

    client.println("</head><body><div class='container'>");

    client.println("<h1>Controle Remoto da Horta</h1>");

    // STATUS DA VÁLVULA
    client.println("<div class='status-box'>");
    if (valvulaEstado == "off") {
        client.println("<p>Status da válvula: <span class='status-text-off'>Desligada</span></p>");
        client.println("<p><a href='/valvula/on'>Ligar</a></p>");
    } else {
        client.println("<p>Status da válvula: <span class='status-text-on'>Ligada</span></p>");
        client.println("<p><a href='/valvula/off'>Desligar</a></p>");
    }
    client.println("</div>");

    // DATA E HORA
    client.println("<div class='card'>");
    client.println("<h2>Data e Hora</h2>");
    client.printf("<p><b>Data:</b> %s</p>", dataAtual.c_str());
    client.printf("<p><b>Hora:</b> %s</p>", horarioAtual.c_str());
    client.println("</div>");

        // PRÓXIMO DISPARO
    client.println("<div class='card'>");
    client.println("<h2>Próxima Irrigação</h2>");
    client.printf("<p><b>Próximo disparo:</b> %s</p>", proximoDisparo.c_str());
    client.printf("<p><b>Duração:</b> %s</p>", duracaoProgramada.c_str());
    client.printf("<p><b>Ciclo:</b> %s</p>", cicloTexto.c_str());
    client.printf("<p><b>Número de ativações:</b> %d</p>", getNumeroAtivacao());
    client.println("</div>");

    // SENSORES
    client.println("<div class='card'>");
    client.println("<h2>Sensores</h2>");
    client.printf("<p><b>Temperatura:</b> %.1f °C</p>", temperatura);
    client.printf("<p><b>Umidade:</b> %.1f %%</p>", umidade);
    client.printf("<p><b>Fluxo Atual:</b> %.2f L/min</p>", fluxoAtual);
    client.printf("<p><b>Total Irrigado:</b> %.2f L</p>", fluxoTotal);
    client.println("</div>");

    // CONTROLE DE CICLO + CONFIGURAR
    client.println("<div class='card'>");
    client.println("<h2>Controle</h2>");
    client.println("<p>Interromper irrigação automática ou abrir a tela de configuração.</p>");
    client.println("<a href='/parar'><button style='background-color:#dc3545'>Parar Ciclo</button></a>");
    client.println("<br><br>");
    client.println("<a href='/config'><button style='background-color:#007bff'>Configurar</button></a>");
    client.println("</div>");

    client.println("</div></body></html>");
}
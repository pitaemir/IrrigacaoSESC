# ESP32 Smart Irrigation Controller

![ESP32](https://img.shields.io/badge/ESP32-Embedded-red)
![Arduino](https://img.shields.io/badge/Framework-Arduino-blue)
![C++](https://img.shields.io/badge/Language-C++-orange)
![Status](https://img.shields.io/badge/Status-Completed-success)

Sistema embarcado para **controle inteligente de irrigação** utilizando **ESP32**, sensores ambientais e um **servidor web embarcado** para monitoramento e configuração.

O projeto foi desenvolvido utilizando **arquitetura modular em C++**, permitindo fácil manutenção, expansão e reutilização de componentes.


# Visão Geral

Este sistema automatiza a irrigação controlando uma **válvula ou bomba de água** através de um **relé**, baseado em:

- horários programados
- monitoramento de sensores
- controle manual via interface web

O dispositivo também monitora **temperatura, umidade e fluxo de água**, permitindo acompanhar o funcionamento do sistema em tempo real.


# Funcionalidades

✔ Controle automático de irrigação  
✔ Interface web para configuração  
✔ Monitoramento de temperatura e umidade  
✔ Medição do fluxo de água  
✔ Agendamento com relógio RTC  
✔ Armazenamento persistente das configurações  
✔ Arquitetura modular em C++  


# Arquitetura do Sistema

O software foi estruturado em módulos independentes para separar responsabilidades do sistema.

```
src/
│
├── main.cpp
│
├── Controle
│   Lógica principal de irrigação
│
├── ServidorWeb
│   Servidor HTTP embarcado
│
├── RTC
│   Controle do relógio de tempo real
│
├── Rele
│   Controle da válvula ou bomba
│
├── WaterFlow
│   Sensor de fluxo de água
│
└── ConfiguracaoPersistente
    Armazenamento de configurações
```

Cada módulo encapsula funcionalidades específicas do hardware ou da lógica do sistema.



# Hardware Utilizado

| Componente | Função |
|------------|--------|
ESP32 | Microcontrolador principal |
DHT11 / DHT22 | Sensor de temperatura e umidade |
DS3231 | Relógio de tempo real (RTC) |
Relay Module | Controle da válvula ou bomba |
Water Flow Sensor | Medição do fluxo de água |


# Pinout Utilizado (Exemplo)

| Componente | GPIO ESP32 |
|------------|-----------|
DHT Sensor | GPIO 4 |
Relé | GPIO 18 |
Sensor de Fluxo | GPIO 2 |
RTC DS3231 | I2C (GPIO 21 / GPIO 22) |

> Os pinos podem ser alterados conforme necessidade do hardware.


# Interface Web

O ESP32 executa um **servidor web interno**, permitindo acessar o sistema diretamente pelo navegador.

A interface permite:

- configurar horários de irrigação
- visualizar dados dos sensores
- ativar/desativar o sistema
- monitorar o estado atual da irrigação

O dispositivo pode funcionar como:

- **Access Point (AP)**
- **cliente conectado à rede WiFi**


# Sensores

## Temperatura e Umidade

Medidas utilizando sensor **DHT11/DHT22**.

Permite monitoramento ambiental e futuras melhorias na lógica de irrigação baseada em clima.


## Fluxo de Água

O sensor de fluxo permite:

- medir o volume de água utilizado
- detectar falhas no sistema hidráulico
- monitorar consumo de irrigação


# Agendamento de Irrigação

O sistema utiliza o **DS3231 RTC**, garantindo precisão no horário mesmo em caso de reinicialização.

Funcionalidades:

- ciclos de irrigação programados
- controle automático do relé
- sincronização do horário do sistema


# Armazenamento Persistente

As configurações são armazenadas em memória não volátil, permitindo que o sistema mantenha seus parâmetros mesmo após reinicialização.

Configurações armazenadas incluem:

- horários de irrigação
- duração dos ciclos
- parâmetros do sistema

---

# Como Compilar e Rodar

### 1️⃣ Instalar dependências

Utilize **Arduino IDE** ou **PlatformIO**.

Bibliotecas necessárias:

- `DHT sensor library`
- `RTClib`


### 2️⃣ Clonar o repositório

```bash
git clone https://github.com/pitaemir/IrrigacaoSESC.git
```


### 3️⃣ Compilar e enviar para o ESP32

Conecte o ESP32 via USB, descomente a linha referente ao preenchimento da hora atual do RTC (//rtc.ajustarHorario(2026, 3, 13,//   15, 17, 0);), e faça o upload do firmware, comente a linha novamente e faça o upload novamente.


### 4️⃣ Acessar interface web

Após iniciar o sistema, conecte-se à rede WiFi criada pelo ESP32 ou à rede configurada.

Abra no navegador:

```
http://192.168.4.1
```

---

# Melhorias Futuras

- Aplicativo mobile
- Integração com serviços cloud
- Dashboard com histórico de dados
- OTA (atualização remota)
- Sensores de umidade do solo
- Controle de múltiplas zonas de irrigação


# Autor

Projeto desenvolvido para estudo e aplicação prática de:

- **Sistemas embarcados**
- **Automação**
- **Internet das Coisas (IoT)**
- **Arquitetura modular em C++**

---

## 🧠 Funcionalidades

- 🌞 Ativação automática diária em horário programado (ex: 16h50)
- ⏱️ Temporização precisa com uso de RTC DS3231
- 💡 Simulação da válvula de irrigação com LED
- 🔁 Reagendamento automático do alarme para o dia seguinte
- ⚙️ Suporte a interrupções com o pino SQW do RTC

## 🛠️ Componentes Utilizados

- Arduino Uno (ou compatível)
- Módulo RTC DS3231
- LED (simulação da válvula)
- Resistor 220Ω
- Jumpers e protoboard
- (Opcional) Relé 5V para acionamento real da válvula

## ⚡ Conexões

| Componente     | Pino Arduino |
|----------------|--------------|
| DS3231 SDA     | A4           |
| DS3231 SCL     | A5           |
| DS3231 SQW     | D2           |
| LED + Resistor | D3           |

> ⚠️ **Importante**: o módulo DS3231 deve estar com bateria (CR2032) para manter a hora mesmo com o Arduino desligado.

## 🚀 Como Usar

1. Instale a biblioteca [`RTClib`](https://github.com/adafruit/RTClib) no Arduino IDE.
2. Conecte os componentes conforme a tabela acima.
3. Faça upload do código `main.ino` para sua placa.
4. O sistema agendará o alarme para o horário programado e repetirá diariamente.

## 🧪 Simulação

Para simular este projeto online, você pode usar o [Wokwi](https://wokwi.com/), que possui suporte ao módulo DS3231.

## 📝 Licença

Este projeto está licenciado sob a [MIT License](LICENSE).

---

Desenvolvido com 💚 por [@pitaemir](https://github.com/pitaemir)

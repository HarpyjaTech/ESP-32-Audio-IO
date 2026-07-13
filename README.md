# Hub de Áudio Multiprotocolo com ESP32 🎧📡

Este projeto transforma um ESP32 padrão num avançado "Servidor de Áudio" distribuído, capaz de operar como Host para aplicações clientes (PC/Mobile), receptor de música sem fio ou roteador de sinal[cite: 6]. 

Desenvolvido para capturar áudio de mesas de som e enviar retornos com qualidade de CD (44.1kHz/16-bits)[cite: 6]. O dispositivo inclui um portal cativo web para configuração inicial e um visor OLED integrado para monitorização de métricas em tempo real (VU Meter, conexões ativas e IP)[cite: 4, 6].

## 🚀 Funcionalidades Principais
* **Máquina de Estados com 3 Modos de Operação**, selecionáveis fisicamente por uma chave deslizante de 3 posições[cite: 6]:
  1. **Servidor TCP:** Hospeda conexões na porta 7000, roteando áudio bidirecional para até 4 aplicativos/clientes simultâneos[cite: 6].
  2. **Receptor Bluetooth (A2DP):** Atua como uma caixa de som Bluetooth de alta fidelidade (desliga o rádio Wi-Fi temporariamente para evitar falhas de transmissão)[cite: 6].
  3. **Auxiliar (Pass-through):** Roteia o áudio capturado na entrada diretamente para a saída digital com latência quase nula[cite: 6].
* **Captura de áudio** usando o módulo ADC **PCM1808** via I2S[cite: 6].
* **Reprodução de retorno** usando o módulo DAC **PCM5102A** via I2S[cite: 6].
* **Interface de rede web** assíncrona para verificar estado e gerir o rádio[cite: 6].

---

## 🛠️ Hardware Necessário

O mapeamento de hardware foi desenvolvido para tirar partido das breakout boards, facilitando a montagem sem necessidade de soldar componentes SMD diminutos[cite: 7].

1. **Placa de Desenvolvimento:** ESP32 (WROOM/WROVER) com Ecrã OLED 0.96" Integrado (I2C)[cite: 6, 7].
2. **Módulo ADC (Entrada da Mesa):** Placa PCM1808 (24-Bits)[cite: 6, 7].
3. **Módulo DAC (Saída para Mesa):** Módulo PCM5102A[cite: 6, 7].
4. **Seletor Físico:** 1x Chave Deslizante de 3 Posições (Slide Switch SP3T ou similar)[cite: 6].
5. **Alimentação:** Fonte 5V / 2A (mínimo)[cite: 7].
6. **Conectores:** 
   * 2x Conectores Jack 6.35mm (P10) Fêmea de Painel para *Line In*[cite: 7].
   * 1x Conector Jack 3.5mm (P2) ou 6.35mm (P10) para *Line Out*[cite: 7].
   * Fios de Áudio Blindados (Shielded Cable)[cite: 7].

---

## 🔌 Esquema de Ligações (Pinout)

O código foi otimizado para evitar conflito de barramentos entre o display OLED interno e as linhas do I2S[cite: 6].

### 1. Interface de Controle
| Componente | Pino do Componente | Pino do ESP32 | Função |
| :--- | :--- | :--- | :--- |
| **Visor OLED** | SDA | **GPIO 21** | Dados I2C[cite: 6] |
| **Visor OLED** | SCL | **GPIO 22** | Relógio I2C[cite: 6] |
| **Chave Seletora** | Pino Lateral A | **GPIO 4** | Seleciona Modo Servidor[cite: 6] |
| **Chave Seletora** | Pino Lateral B | **GPIO 5** | Seleciona Modo Auxiliar[cite: 6] |
| **Chave Seletora** | Pino Central (Comum)| **GND** | Aterra a seleção desejada[cite: 6] |

*(Nota: O Modo Bluetooth é ativado automaticamente quando a chave está no centro e nenhum dos pinos laterais está aterrado[cite: 6]).*

### 2. Barramentos de Áudio I2S
A placa ESP32 deve alimentar os módulos PCM de áudio apenas com `3.3V` e partilhar a ligação `GND` comum[cite: 7].

**Entrada Analógica (PCM1808 - Apenas RX):**
* **BCK**  $\rightarrow$ **GPIO 14**[cite: 6]
* **LRCK** $\rightarrow$ **GPIO 12**[cite: 6]
* **DOUT** $\rightarrow$ **GPIO 13**[cite: 6]

**Saída Analógica (PCM5102A - Apenas TX):**
* **BCK**  $\rightarrow$ **GPIO 26**[cite: 6]
* **LCK**  $\rightarrow$ **GPIO 25**[cite: 6]
* **DIN**  $\rightarrow$ **GPIO 18**[cite: 6]

---

## ⚙️ Como Instalar e Usar

### 1. Preparação do Firmware
1. Abra o projeto no **Arduino IDE**[cite: 7].
2. Nas opções de `Ferramentas > Partition Scheme`, selecione **"Huge APP"** ou **"Minimal SPIFFS"**, pois as bibliotecas de Wi-Fi, Servidor e Bluetooth agrupadas requerem espaço extra na Flash[cite: 6].
3. Instale as bibliotecas necessárias: `Adafruit_GFX`, `Adafruit_SSD1306`[cite: 6] e **`ESP32-A2DP`** (do autor Phil Schatzmann)[cite: 6].
4. Faça o upload para o ESP32[cite: 7].

### 2. Configuração Wi-Fi (Modo Access Point)
* Ao ligar pela primeira vez (ou se a rede configurada cair), o dispositivo criará uma rede Wi-Fi temporária chamada **`AudioServer_Setup`**[cite: 6].
* Conecte-se a ela com o seu smartphone[cite: 7].
* Navegue para **`192.168.4.1`** no seu browser[cite: 4, 6].
* Selecione a rede local (SSID) da sua Igreja/Estúdio, digite a senha e guarde[cite: 4, 6]. O ESP32 reiniciará[cite: 4].

### 3. Conexão dos Clientes (Aplicações)
O ESP32 atua como o servidor primário na rede[cite: 6]. Para que as suas futuras aplicações clientes funcionem:
* Certifique-se de que a chave está na posição **"Modo Servidor"**[cite: 6].
* O endereço de ligação será o IP mostrado no visor OLED[cite: 6].
* A aplicação deve abrir um Socket TCP na porta **`7000`**[cite: 4, 6].
* O fluxo de áudio consiste num stream contínuo de PCM cru a `44100Hz`, `16-bits`, Estéreo[cite: 6].

---
*Arquitetura refatorada para ESP32 em C++.*
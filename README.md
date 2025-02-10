
---

# Tarefa - Comunicação Serial e Controle de LEDs com RP2040

## 📌 Descrição
Este projeto consolida os conceitos sobre o uso de interfaces de comunicação serial no RP2040 e explora as funcionalidades da placa de desenvolvimento **BitDogLab**. Utilizando comunicação **UART** e **I2C**, o projeto combina o controle de uma matriz de LEDs endereçáveis, LEDs RGB e um display SSD1306.

## 🛠️ Componentes Utilizados
- **Microcontrolador:** Raspberry Pi Pico W (Placa BitDogLab)
- **Matriz 5x5 de LEDs (endereçáveis) WS2812** conectada à GPIO 7
- **LED RGB** com os pinos conectados às GPIOs 11, 12, e 13
- **Botão A** conectado à GPIO 5
- **Botão B** conectado à GPIO 6
- **Display SSD1306** conectado via I2C (GPIO 14 e GPIO 15)

## 🔥 Funcionalidades
1. **Modificação da Biblioteca font.h**:
    - Adição de caracteres minúsculos.

2. **Entrada de caracteres via PC**:
    - Cada caractere digitado no Serial Monitor do VS Code é exibido no display SSD1306.
    - Quando um número entre 0 e 9 é digitado, o símbolo correspondente é exibido na matriz 5x5 WS2812.

3. **Interação com o Botão A**:
    - Alterna o estado do LED RGB Verde (ligado/desligado).
    - Exibe uma mensagem no display SSD1306 indicando o estado do LED.
    - Envia um texto descritivo ao Serial Monitor.

4. **Interação com o Botão B**:
    - Alterna o estado do LED RGB Azul (ligado/desligado).
    - Exibe uma mensagem no display SSD1306 indicando o estado do LED.
    - Envia um texto descritivo ao Serial Monitor.

## 🚀 Requisitos do Projeto
1. **Uso de Interrupções**:
    - Todas as funcionalidades relacionadas aos botões são implementadas utilizando rotinas de interrupção (IRQ).

2. **Debouncing**:
    - Implementação de tratamento de bouncing dos botões via software.

3. **Controle de LEDs**:
    - Uso de LEDs comuns e LEDs WS2812, demonstrando diferentes tipos de controle.

4. **Utilização do Display 128x64**:
    - Uso de fontes maiúsculas e minúsculas, entendimento do funcionamento do display e utilização do protocolo I2C.

5. **Envio de Informação pela UART**:
    - Compreensão da comunicação serial via UART.

6. **Organização do Código**:
    - Código bem estruturado e comentado para facilitar o entendimento.

## 🖥️ Como Executar o Projeto

### Passo 1: Clone o repositório do projeto para o seu ambiente local e abra-o no VS Code.

### Passo 2: Configurar o Ambiente
Garanta que o **Pico SDK** está instalado corretamente e que o **VS Code** está configurado com a extensão **Raspberry Pi Pico**.

### Passo 3: Compilar o Código
Compile o projeto no VS Code utilizando a extensão Raspberry Pi Pico.

### Passo 4: Carregar o Código na Placa
1. Conecte a placa **BitDogLab** via USB.
2. Coloque-a no modo **bootsel**.
3. Use a opção "**Run Project (USB)**" da extensão para carregar o arquivo `.uf2`.

### Passo 5: Verificar o Funcionamento
1. Digite caracteres no Serial Monitor e observe-os no display SSD1306.
2. Digite números de 0 a 9 para visualizar os símbolos correspondentes na matriz de LEDs.
3. Pressione o **Botão A** para alternar o estado do LED Verde e verificar as mensagens no display e no Serial Monitor.
4. Pressione o **Botão B** para alternar o estado do LED Azul e verificar as mensagens no display e no Serial Monitor.

## 📄 Estrutura do Projeto
- `main.c` → Implementação principal do controle dos LEDs, botões e display.
- `CMakeLists.txt` → Configuração do CMake para compilação do projeto.

# Link para o vídeo de demonstração

https://drive.google.com/drive/folders/1lsMfmo8AEE4ACxMUmJ7l_WuXw7q5oCHg?usp=sharing

## 📌 Autor
Projeto desenvolvido por **Jeová Pinheiro** para a fase 1 do ***EmbarcaTech***.

---
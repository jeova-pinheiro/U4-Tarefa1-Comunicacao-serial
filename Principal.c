#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/clocks.h"
#include "hardware/pio.h"
#include "pio_led.pio.h"
#include "hardware/timer.h"
#include "lib/font.h"
#include "lib/ssd1306.h"
#include "hardware/uart.h"

// Definindo os pinos e outras constantes
const uint PIN_SAIDA = 7, PIN_BTN_X = 5, PIN_BTN_Y = 6, PIN_LED_AZUL = 12, PIN_LED_VERDE = 11, I2C_PIN_CLK = 15, I2C_PIN_DATA = 14, UART_PIN_TX = 0, UART_PIN_RX = 1;
const uint MATRIZ_DIMENSAO = 5;
#define I2C_PORTA i2c1
#define ENDERECO 0x3C

// Definindo um apelido para o tipo "unsigned char"
typedef unsigned char uchar;

// Variáveis para armazenar o tempo do último evento dos botões
uint32_t ULTIMO_EVENTO_X = 0, ULTIMO_EVENTO_Y = 0;

// Matriz contendo os padrões de desenhos dos números
const float desenhos[10][5][5] = {

    // Desenhos a serem usados nos padrões de LEDs

    {{0.1, 0.1, 0.1, 0.1, 0.1},
     {0.1, 0.0, 0.0, 0.0, 0.1},
     {0.1, 0.0, 0.0, 0.0, 0.1},
     {0.1, 0.0, 0.0, 0.0, 0.1},
     {0.1, 0.1, 0.1, 0.1, 0.1}},

    {{0.0, 0.0, 0.1, 0.0, 0.0},
     {0.0, 0.1, 0.1, 0.0, 0.0},
     {0.1, 0.0, 0.1, 0.0, 0.0},
     {0.0, 0.0, 0.1, 0.0, 0.0},
     {0.0, 0.1, 0.1, 0.1, 0.0}},

    {{0.1, 0.1, 0.1, 0.1, 0.1},
     {0.0, 0.0, 0.0, 0.0, 0.1},
     {0.1, 0.1, 0.1, 0.1, 0.1},
     {0.1, 0.0, 0.0, 0.0, 0.0},
     {0.1, 0.1, 0.1, 0.1, 0.1}},

    {{0.1, 0.1, 0.1, 0.1, 0.1},
     {0.0, 0.0, 0.0, 0.0, 0.1},
     {0.1, 0.1, 0.1, 0.1, 0.1},
     {0.0, 0.0, 0.0, 0.0, 0.1},
     {0.1, 0.1, 0.1, 0.1, 0.1}},

    {{0.1, 0.0, 0.0, 0.0, 0.1},
     {0.1, 0.0, 0.0, 0.0, 0.1},
     {0.1, 0.1, 0.1, 0.1, 0.1},
     {0.0, 0.0, 0.0, 0.0, 0.1},
     {0.0, 0.0, 0.0, 0.0, 0.1}},

    {{0.1, 0.1, 0.1, 0.1, 0.1},
     {0.1, 0.0, 0.0, 0.0, 0.0},
     {0.1, 0.1, 0.1, 0.1, 0.1},
     {0.0, 0.0, 0.0, 0.0, 0.1},
     {0.1, 0.1, 0.1, 0.1, 0.1}},

    {{0.1, 0.1, 0.1, 0.1, 0.1},
     {0.1, 0.0, 0.0, 0.0, 0.0},
     {0.1, 0.1, 0.1, 0.1, 0.1},
     {0.1, 0.0, 0.0, 0.0, 0.1},
     {0.1, 0.1, 0.1, 0.1, 0.1}},

    {{0.1, 0.1, 0.1, 0.1, 0.1},
     {0.0, 0.0, 0.0, 0.1, 0.0},
     {0.0, 0.0, 0.1, 0.0, 0.0},
     {0.0, 0.1, 0.0, 0.0, 0.0},
     {0.1, 0.0, 0.0, 0.0, 0.0}},

    {{0.1, 0.1, 0.1, 0.1, 0.1},
     {0.1, 0.0, 0.0, 0.0, 0.1},
     {0.1, 0.1, 0.1, 0.1, 0.1},
     {0.1, 0.0, 0.0, 0.0, 0.1},
     {0.1, 0.1, 0.1, 0.1, 0.1}},

    {{0.1, 0.1, 0.1, 0.1, 0.1},
     {0.1, 0.0, 0.0, 0.0, 0.1},
     {0.1, 0.1, 0.1, 0.1, 0.1},
     {0.0, 0.0, 0.0, 0.0, 0.1},
     {0.0, 0.0, 0.0, 0.0, 0.1}},
};

PIO pioControl;     // Controlador PIO
uint offsetCodigo;  // Deslocamento do código
uint estadoMaquina; // Máquina de estado
ssd1306_t display;  // Display SSD1306

// Função que retorna a cor RGB em formato de um inteiro de 32 bits
uint32_t corRGB(float vermelho, float verde, float azul)
{
    // Garante que os valores dos parâmetros estejam no intervalo [0, 1]
    uchar R = (vermelho > 1 || vermelho < 0 ? 0.1 : vermelho) * 255;
    uchar G = (verde > 1 || verde < 0 ? 0.1 : verde) * 255;
    uchar B = (azul > 1 || azul < 0 ? 0.1 : azul) * 255;

    return (R << 24) | (G << 16) | (B << 8);
}

// Funções declaradas anteriormente
void exibirDesenhoNumero(PIO *pioControlador, uint *estadoMaquina, int indiceDesenho);
void manipuladorInterrupcaoGPIO(uint gpio, uint32_t eventos);

// Configuração inicial do projeto
void configurarProjeto();

int main()
{
    stdio_init_all(); // Inicializa todas as bibliotecas stdio

    configurarProjeto(); // Configura o projeto

    while (true)
    {
        if (stdio_usb_connected())
        {
            char caractere = '\0';
            // Lê um caractere da entrada padrão
            if (scanf("%c", &caractere) == 1)
            {
                if (caractere >= '0' && caractere <= '9')
                {
                    exibirDesenhoNumero(&pioControl, &estadoMaquina, caractere - '0');
                }
            }
            // Define a mensagem a ser exibida no display
            char mensagem[] = "Caractere Digitado  \0";
            mensagem[(sizeof(mensagem) / sizeof(char)) - 2] = caractere;
            ssd1306_draw_string(&display, mensagem, 1, 1);
            ssd1306_send_data(&display);
        }
    }
    sleep_ms(50); // Aguarda por 50 milissegundos
    return 0;
}

// Função para exibir o desenho do número nos LEDs
void exibirDesenhoNumero(PIO *pioControlador, uint *estadoMaquina, int indiceDesenho)
{
    // Verifica se o índice do desenho é inválido
    if (indiceDesenho == -1)
        return;
    // Acende os LEDs conforme o padrão do desenho na matriz
    for (int j = 0; j < MATRIZ_DIMENSAO; j++)
    {
        for (int k = 0; k < MATRIZ_DIMENSAO; k++)
        {
            // Envia as informações de cor e intensidade do LED para a máquina de estado
            pio_sm_put_blocking(*pioControlador, *estadoMaquina, corRGB(desenhos[indiceDesenho][MATRIZ_DIMENSAO - 1 - j][(j + 1) % 2 == 0 ? k : MATRIZ_DIMENSAO - k - 1], 0.0, 0.0));
        }
    }
}

// Manipulador de interrupção GPIO
void manipuladorInterrupcaoGPIO(uint gpio, uint32_t eventos)
{
    uint32_t tempo_atual = to_us_since_boot(get_absolute_time()); // Obtém o tempo atual

    if (gpio == PIN_BTN_X)
    {
        // Condição para debounce do botão X
        if (tempo_atual - ULTIMO_EVENTO_X > 200000)
        {
            ULTIMO_EVENTO_X = tempo_atual;
            gpio_put(PIN_LED_VERDE, !gpio_get(PIN_LED_VERDE));
            ssd1306_draw_string(&display, gpio_get(PIN_LED_VERDE) ? "LED VERDE ligado\0" : "LED VERDE desligado\0", 1, 24);
            ssd1306_send_data(&display);
        }
    }
    else if (gpio == PIN_BTN_Y)
    {
        // Condição para debounce do botão Y
        if (tempo_atual - ULTIMO_EVENTO_Y > 200000)
        {
            ULTIMO_EVENTO_Y = tempo_atual;
            gpio_put(PIN_LED_AZUL, !gpio_get(PIN_LED_AZUL));
            ssd1306_draw_string(&display, gpio_get(PIN_LED_AZUL) ? "LED AZUL ligado\0" : "LED AZUL desligado\0", 1, 40);
            ssd1306_send_data(&display);
        }
    }
}

// Configuração inicial do projeto
void configurarProjeto()
{
    // Início da configuração inicial do firmware
    pioControl = pio0;
    set_sys_clock_khz(128000, false);

    gpio_init(PIN_BTN_X);
    gpio_init(PIN_BTN_Y);
    gpio_init(PIN_LED_VERDE);
    gpio_init(PIN_LED_AZUL);

    gpio_set_dir(PIN_BTN_X, GPIO_IN);
    gpio_set_dir(PIN_BTN_Y, GPIO_IN);
    gpio_set_dir(PIN_LED_VERDE, GPIO_OUT);
    gpio_set_dir(PIN_LED_AZUL, GPIO_OUT);

    gpio_pull_up(PIN_BTN_X);
    gpio_pull_up(PIN_BTN_Y);

    // Configuração do PIO
    offsetCodigo = pio_add_program(pioControl, &pio_led_program);
    estadoMaquina = pio_claim_unused_sm(pioControl, true);
    pio_led_program_init(pioControl, estadoMaquina, offsetCodigo, PIN_SAIDA);

    // Habilita a interrupção no GPIO
    gpio_set_irq_enabled_with_callback(PIN_BTN_X, GPIO_IRQ_EDGE_FALL, true, &manipuladorInterrupcaoGPIO);
    gpio_set_irq_enabled_with_callback(PIN_BTN_Y, GPIO_IRQ_EDGE_FALL, true, &manipuladorInterrupcaoGPIO);

    // Fim da configuração inicial do firmware

    // Configuração do I2C e do monitor
    i2c_init(I2C_PORTA, 400 * 1000);

    gpio_set_function(I2C_PIN_DATA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_PIN_CLK, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_PIN_DATA);
    gpio_pull_up(I2C_PIN_CLK);

    ssd1306_init(&display, WIDTH, HEIGHT, false, ENDERECO, I2C_PORTA); // Inicializa o display
    ssd1306_config(&display);                                          // Configura o display
    ssd1306_send_data(&display);                                       // Envia os dados para o display

    // Limpa o display. O display inicia com todos os pixels apagados.
    ssd1306_fill(&display, false);
    ssd1306_send_data(&display);

    // Configuração do UART para testes no wokwi
    uart_init(uart0, 115200);
    gpio_set_function(UART_PIN_RX, GPIO_FUNC_UART);
    gpio_set_function(UART_PIN_TX, GPIO_FUNC_UART);
    uart_set_fifo_enabled(uart0, true);

    // Inicializa todos os LEDs apagados
    for (int k = 0; k < 25; k++)
    {
        // Envia as informações de cor e intensidade do LED para a máquina de estado
        pio_sm_put_blocking(pioControl, estadoMaquina, 0.0);
    }
}

#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "hardware/clocks.h"
#include "pico/bootrom.h"
#include "hardware/pio.h"
#include "vogal.pio.h" // Arquivo .pio atualizado para a matriz

// Configurações do teclado matricial 4x4
#define ROWS 4
#define COLS 4
const uint8_t row_pins[ROWS] = {17, 16, 18, 19};
const uint8_t col_pins[COLS] = {20, 4, 9, 8};
const char key_map[ROWS][COLS] = {
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'}
};

// Configurações da matriz de LEDs
#define NUM_PIXELS 25
#define OUT_PIN 7

// Configurações do buzzer
#define BUZZER_PIN 21
#define BUZZER_FREQ 4000 // Frequência do buzzer em Hz

// Notas musicais: Dó, Ré, Mi, Fá, Sol, Lá, Si, Dó
const int melody[] = {262, 294, 330, 349, 392, 440, 494, 523};
const int melody_duration[] = {300, 300, 300, 300, 300, 300, 300, 600};
const int melody_length = 8;

// Representação das letras A, E, I, O, U em matrizes
const int letter_frames[5][25] = {
    // Letra A
    { 0,  1,  1,  1,  0,
      1,  0,  0,  0,  1,
      1,  1,  1,  1,  1,
      1,  0,  0,  0,  1,
      1,  0,  0,  0,  1},

    // Letra E
    { 1,  1,  1,  1,  1,
      0,  0,  0,  0,  1,
      1,  1,  1,  1,  1,
      0,  0,  0,  0,  1,
      1,  1,  1,  1,  1},

    // Letra I
    { 0,  0,  1,  0,  0,
      0,  0,  1,  0,  0,
      0,  0,  1,  0,  0,
      0,  0,  1,  0,  0,
      0,  0,  1,  0,  0},

    // Letra O
    { 0,  1,  1,  1,  0,
      1,  0,  0,  0,  1,
      1,  0,  0,  0,  1,
      1,  0,  0,  0,  1,
      0,  1,  1,  1,  0},

    // Letra U
    { 1,  0,  0,  0,  1,
      1,  0,  0,  0,  1,
      1,  0,  0,  0,  1,
      1,  0,  0,  0,  1,
      0,  1,  1,  1,  0}
};

// Protótipos de Funções
void init_gpio();
char scan_keypad();
void executar_acao(char key, PIO pio, uint sm);
void setup_buzzer_pwm(uint pin, uint freq_hz);
void play_buzzer(int times, uint freq_hz, uint duration_ms);
void play_melody();
void render_letter(const int *letter, PIO pio, uint sm);

// Função principal
int main() {
    stdio_init_all();
    PIO pio = pio0;

    uint offset = pio_add_program(pio, &pio_matrix_program);
    uint sm = pio_claim_unused_sm(pio, true);
    pio_matrix_program_init(pio, sm, offset, OUT_PIN);

    init_gpio();
    setup_buzzer_pwm(BUZZER_PIN, BUZZER_FREQ);

    printf("Sistema inicializado. Pressione teclas no teclado matricial.\n");

    while (1) {
        char key = scan_keypad();
        if (key != '\0') {
            printf("Tecla pressionada: %c\n", key);
            executar_acao(key, pio, sm);
        }
        sleep_ms(100);
    }
    return 0;
}

// Inicializa GPIOs do teclado matricial
void init_gpio() {
    for (int i = 0; i < ROWS; i++) {
        gpio_init(row_pins[i]);
        gpio_set_dir(row_pins[i], GPIO_OUT);
        gpio_put(row_pins[i], 1);
    }
    for (int i = 0; i < COLS; i++) {
        gpio_init(col_pins[i]);
        gpio_set_dir(col_pins[i], GPIO_IN);
        gpio_pull_up(col_pins[i]);
    }
}

// Verifica tecla pressionada
char scan_keypad() {
    for (int row = 0; row < ROWS; row++) {
        gpio_put(row_pins[row], 0);
        for (int col = 0; col < COLS; col++) {
            if (gpio_get(col_pins[col]) == 0) {
                while (gpio_get(col_pins[col]) == 0);
                gpio_put(row_pins[row], 1);
                return key_map[row][col];
            }
        }
        gpio_put(row_pins[row], 1);
    }
    return '\0';
}

// Renderiza letras na matriz de LEDs
void render_letter(const int *letter, PIO pio, uint sm) {
    uint32_t frame[NUM_PIXELS] = {0};
    for (int i = 0; i < NUM_PIXELS; i++) {
        if (letter[i]) {
            frame[i] = (0xFF << 16); // Vermelho
        }
        for (int j = 0; j < NUM_PIXELS; j++) {
            pio_sm_put_blocking(pio, sm, frame[24 - j]);
        }
        sleep_ms(100);
    }
    sleep_ms(500);
}

// Configura PWM no pino do buzzer
void setup_buzzer_pwm(uint pin, uint freq_hz) {
    gpio_set_function(pin, GPIO_FUNC_PWM);
    uint slice_num = pwm_gpio_to_slice_num(pin);
    pwm_set_clkdiv(slice_num, clock_get_hz(clk_sys) / (freq_hz * 4096));
    pwm_set_enabled(slice_num, true);
}

// Toca o buzzer um número de vezes
void play_buzzer(int times, uint freq_hz, uint duration_ms) {
    uint slice_num = pwm_gpio_to_slice_num(BUZZER_PIN);
    for (int i = 0; i < times; i++) {
        pwm_set_chan_level(slice_num, pwm_gpio_to_channel(BUZZER_PIN), 2048);
        sleep_ms(duration_ms);
        pwm_set_chan_level(slice_num, pwm_gpio_to_channel(BUZZER_PIN), 0);
        sleep_ms(200);
    }
}

// Toca uma melodia
void play_melody() {
    uint slice_num = pwm_gpio_to_slice_num(BUZZER_PIN);
    for (int i = 0; i < melody_length; i++) {
        pwm_set_clkdiv(slice_num, clock_get_hz(clk_sys) / (melody[i] * 4096));
        pwm_set_chan_level(slice_num, pwm_gpio_to_channel(BUZZER_PIN), 2048);
        sleep_ms(melody_duration[i]);
        pwm_set_chan_level(slice_num, pwm_gpio_to_channel(BUZZER_PIN), 0);
        sleep_ms(150);
    }
}

// Executa ações com base na tecla pressionada
void executar_acao(char key, PIO pio, uint sm) {
    uint32_t color; // Armazena a cor para cada ação
    switch (key) {
        case '1': // Exibe animação e toca a melodia
            play_buzzer(2, BUZZER_FREQ, 200); // Toca o buzzer duas vezes
            for (int i = 0; i < 5; i++) { // Exibe cada letra
                render_letter(letter_frames[i], pio, sm);
            }
            play_melody(); // Toca a melodia
            break;

        case 'A': // Desliga todos os LEDs
            for (int i = 0; i < NUM_PIXELS; i++) {
                pio_sm_put_blocking(pio, sm, 0); // Cor preta (desligado)
            }
            printf("Todos os LEDs foram desligados.\n");
            break;

        case 'B': // Acende todos os LEDs em azul com 100% de luminosidade
            color = (0xFF << 8); // Azul em 100% (R=0, G=0, B=255)
            for (int i = 0; i < NUM_PIXELS; i++) {
                pio_sm_put_blocking(pio, sm, color);
            }
            printf("Todos os LEDs estão azuis (100%%).\n");
            break;

        case 'C': // Acende todos os LEDs em vermelho com 50% de luminosidade
            color = (0x80 << 16); // Vermelho em 50% (R=128, G=0, B=0)
            for (int i = 0; i < NUM_PIXELS; i++) {
                pio_sm_put_blocking(pio, sm, color);
            }
            printf("Todos os LEDs estão vermelhos (50%%).\n");
            break;

        case 'D': // Acende todos os LEDs em verde com 50% de luminosidade
            color = (0x80 << 24); // Verde em 50% (R=0, G=128, B=0)
            for (int i = 0; i < NUM_PIXELS; i++) {
                pio_sm_put_blocking(pio, sm, color);
            }
            printf("Todos os LEDs estão verdes (50%%).\n");
            break;

       case '#': // Acende todos os LEDs em branco com 20% de luminosidade
            color = ((0x20 << 16) | (0x40 << 8) | 0x10); // Ajuste para RGB (vermelho: 32, verde: 64, azul: 16)
            for (int i = 0; i < NUM_PIXELS; i++) {
                pio_sm_put_blocking(pio, sm, color);
            }
            printf("Todos os LEDs estão brancos (20%%).\n");
            break;

        case '0': // Reinicia o microcontrolador no modo de gravação
            printf("Reinicializando o microcontrolador para modo de gravação.\n");
            sleep_ms(100); // Pausa breve antes do reinício
            reset_usb_boot(0, 0); // Entra no modo de gravação
            break;

        default:
            printf("Tecla não mapeada.\n");
            break;
    }
}

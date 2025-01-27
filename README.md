# Projeto_3_Anima-o
# Projeto: Geração de Animação em Matriz de LEDs com Raspberry Pi Pico W

## Visão Geral

Este projeto é uma atividade prática desenvolvida como parte das atividades do **EMBARCATECH 2024/25**. Utilizando a placa **BitDogLab** com um microcontrolador **Raspberry Pi Pico W**, o objetivo é criar animações visuais em uma matriz de LEDs RGB (5x5) controlada por um teclado matricial 4x4, com integração de um buzzer passivo.

**Autor do Projeto:**\
Dr. Marlon da Silva Garrido\
Professor Associado IV (CENAMB - PPGEA)\
Universidade Federal do Vale do São Francisco (UNIVASF)

---

## Objetivo

Implementar uma solução que:

1. Utilize o simulador Wokwi para representar a arquitetura do sistema.
2. Controle uma matriz de LEDs RGB (5x5) com base na entrada de um teclado matricial 4x4.
3. Execute diferentes ações, como:
   - Apagar os LEDs.
   - Acender LEDs em cores específicas e intensidades configuráveis.
   - Gerar animações visuais representando letras (A, E, I, O, U).
   - Tocar uma melodia ou som no buzzer passivo.

---

## Componentes do Sistema

### Hardware

- **Microcontrolador:** Raspberry Pi Pico W.
- **Teclado Matricial 4x4:** Para entrada de comandos.
- **Matriz de LEDs RGB 5x5 (WS2812):** Para exibição das animações.
- **Buzzer Passivo (MLT-8530 SMD 3.6V):** Para reprodução de sons e melodia.

### Software

- **Simulador:** Wokwi (simulação de hardware e execução de códigos).
- **Linguagem de Programação:** C.
- **PIO (Programmable I/O):** Controle da matriz de LEDs.

### Vídeo Demonstrativo
[Uso na Placa Física e Simulação com VSCode e Wokwi](https://youtu.be/5EksPTAZwoc)

---

## Funcionalidades do Projeto

O sistema responde às seguintes ações:

### Ações do Teclado Matricial

1. **Tecla ********`1`********:**

   - Inicia a animação exibindo as letras A, E, I, O, U na matriz de LEDs.
   - Toca uma melodia no buzzer.

2. **Tecla ********`A`********:**

   - Desliga todos os LEDs.

3. **Tecla ********`B`********:**

   - Acende todos os LEDs na cor **azul**, com intensidade de 100%.

4. **Tecla ********`C`********:**

   - Acende todos os LEDs na cor **vermelha**, com intensidade de 80%.

5. **Tecla ********`D`********:**

   - Acende todos os LEDs na cor **verde**, com intensidade de 50%.

6. **Tecla ********`#`********:**

   - Acende todos os LEDs na cor **branca**, com intensidade de 20%.

7. **Tecla ********`0`********:**

   - Reinicia o microcontrolador no modo de gravação USB.

### Outros Comportamentos

- **Animações Personalizadas:** Letras (A, E, I, O, U) são renderizadas na matriz de LEDs com transições suaves.
- **Buzzer:** Reprodução de sons e melodias em resposta às entradas do teclado.

---

## Requisitos do Projeto

1. **Hardware Necessário:**

   - Raspberry Pi Pico W.
   - Teclado Matricial 4x4.
   - Matriz de LEDs RGB WS2812 (5x5).
   - Buzzer Passivo MLT-8530 SMD.

2. **Bibliotecas e Dependências:**

   - `pico/stdlib.h`
   - `hardware/pwm.h`
   - `hardware/clocks.h`
   - `hardware/pio.h`
   - `vogal.pio.h`

3. **Configurações Específicas:**

   - Pinos GPIO mapeados para o teclado matricial e a matriz de LEDs.
   - Frequência do buzzer: 4000 Hz.

---

## Estrutura do Código

O código do projeto foi organizado para modularidade e clareza. Aqui está um resumo das principais funções implementadas:

### 1. Inicialização do Sistema

- Configura os GPIOs do teclado matricial.
- Inicializa o PIO para controlar a matriz de LEDs.
- Configura o PWM para o buzzer.

### 2. Controle do Teclado Matricial

A função `scan_keypad` verifica as teclas pressionadas e retorna o caractere correspondente:

```c
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
```

### 3. Renderização na Matriz de LEDs

A função `render_letter` desenha uma letra na matriz de LEDs com base nos frames predefinidos:

```c
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
```

### 4. Controle do Buzzer

As funções `play_buzzer` e `play_melody` permitem reproduzir sons individuais ou melodias completas.

### 5. Execução de Ações

A função `executar_acao` associa cada tecla a uma ação específica (animação, som, cor, etc.).

---

## Como Executar o Projeto

1. **Configuração do Ambiente:**

   - Carregue o código em um simulador (como o Wokwi) ou em um Raspberry Pi Pico W real.

2. **Interação:**

   - Pressione as teclas do teclado matricial para acionar os LEDs e o buzzer.

3. **Validação:**

   - Verifique se as cores e intensidades exibidas na matriz estão corretas.
   - Confirme a reprodução de sons e melodias no buzzer.

---

## Referências

- [Documentação do Raspberry Pi Pico W](https://www.raspberrypi.com/documentation/microcontrollers/)
- [Wokwi Simulator](https://wokwi.com/)
- [Bibliotecas e exemplos da Raspberry Pi Foundation](https://github.com/raspberrypi/pico-sdk)

---

## Contato

Para mais informações ou dúvidas sobre este projeto, entre em contato com:
Dr. Marlon da Silva Garrido - **UNIVASF**\
Email: [marlon.garrido@univasf.edu.br](mailto\:marlon.garrido@univasf.edu.br)


#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "hardware/clocks.h"

// Configuração do pino do buzzer
#define BUZZER_PIN 21

// Configuração da frequência do buzzer (em Hz)
#define BUZZER_FREQUENCY 100

// Configuração do pino do LED vermelho
#define LED_R_PIN 13

// Configuração do pino para o sensor
#define SENSOR 8

// Definição de uma função para inicializar o PWM no pino do buzzer
void pwm_init_buzzer(uint pin) {
    // Configurar o pino como saída de PWM
    gpio_set_function(pin, GPIO_FUNC_PWM);

    // Obter o slice do PWM associado ao pino
    uint slice_num = pwm_gpio_to_slice_num(pin);

    // Configurar o PWM com frequência desejada
    pwm_config config = pwm_get_default_config();
    pwm_config_set_clkdiv(&config, clock_get_hz(clk_sys) / (BUZZER_FREQUENCY * 4096)); // Divisor de clock
    pwm_init(slice_num, &config, true);

    // Iniciar o PWM no nível baixo
    pwm_set_gpio_level(pin, 0);
}

// Definição de uma função para emitir um beep com duração especificada
void beep(uint pin, uint duration_ms) {
    // Obter o slice do PWM associado ao pino
    uint slice_num = pwm_gpio_to_slice_num(pin);

    // Configurar o duty cycle para 50% (ativo)
    pwm_set_gpio_level(pin, 2048);

    // Temporização
    sleep_ms(duration_ms);

    // Desativar o sinal PWM (duty cycle 0)
    pwm_set_gpio_level(pin, 0);

    // Pausa entre os beeps
    sleep_ms(100); // Pausa de 100ms
}

int main() {
    // Inicializar o sistema de saída padrão
    stdio_init_all();

    // Inicializando pino vermelho
    gpio_init(LED_R_PIN);
    gpio_set_dir(LED_R_PIN, GPIO_OUT);
    gpio_put(LED_R_PIN, 0); // Iniciando o LED desligado

    gpio_init(SENSOR);
    gpio_set_dir(SENSOR, GPIO_IN);

    // Inicializar o PWM no pino do buzzer
    pwm_init_buzzer(BUZZER_PIN);

    // Loop infinito
    while (true) {
        int chama = gpio_get(SENSOR);

        while(chama == 1) {
            beep(BUZZER_PIN, 500); // Bipe de 500ms
            gpio_put(LED_R_PIN, 1); // Liga LED vermelho
            chama = gpio_get(SENSOR);
        }

        gpio_put(LED_R_PIN, 0);             // Desliga LED quando não detectado nenhuma chama
        pwm_set_gpio_level(BUZZER_PIN, 0);  // Desliga BUZZER quando não detectado nenhuma chama
        
    }

    return 0;
}


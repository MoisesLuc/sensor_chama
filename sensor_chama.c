#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "hardware/clocks.h"
#include "hardware/pio.h"

// Configuração do pino do buzzer
#define BUZZER_PIN 21

// Configuração da frequência do buzzer (em Hz)
#define BUZZER_FREQUENCY 2000  // Frequência mais apropriada para buzzer

// Configuração do pino do LED vermelho
#define LED_R_PIN 13

// Configuração do pino para o sensor
#define SENSOR 8

// Configurações de debounce
#define DEBOUNCE_TIME_MS 50
#define MAX_DETECTION_TIME_MS 5000  // Tempo máximo de detecção

// Definição de uma função para inicializar o PWM no pino do buzzer
void pwm_init_buzzer(uint pin) {
    // Configurar o pino como saída de PWM
    gpio_set_function(pin, GPIO_FUNC_PWM);

    // Obter o slice do PWM associado ao pino
    uint slice_num = pwm_gpio_to_slice_num(pin);

    // Configurar o PWM com frequência desejada
    pwm_config config = pwm_get_default_config();
    pwm_config_set_clkdiv(&config, clock_get_hz(clk_sys) / (BUZZER_FREQUENCY * 4096));
    pwm_init(slice_num, &config, true);

    // Iniciar o PWM no nível baixo
    pwm_set_gpio_level(pin, 0);
}

// Definição de uma função para emitir um beep com duração especificada
void beep(uint pin, uint duration_ms) {
    uint slice_num = pwm_gpio_to_slice_num(pin);
    pwm_set_gpio_level(pin, 2048);  // 50% duty cycle
    sleep_ms(duration_ms);
    pwm_set_gpio_level(pin, 0);
    sleep_ms(100);  // Pausa entre beeps
}

// Função para verificar se a leitura do sensor é estável
bool is_stable_reading(uint pin, uint debounce_time_ms) {
    int initial_reading = gpio_get(pin);
    sleep_ms(debounce_time_ms);
    return gpio_get(pin) == initial_reading;
}

int main() {
    stdio_init_all();

    // Inicializando pino vermelho
    gpio_init(LED_R_PIN);
    gpio_set_dir(LED_R_PIN, GPIO_OUT);
    gpio_put(LED_R_PIN, 0);

    // Inicializando sensor com pull-down
    gpio_init(SENSOR);
    gpio_set_dir(SENSOR, GPIO_IN);
    gpio_pull_down(SENSOR);

    // Inicializar o PWM no pino do buzzer
    pwm_init_buzzer(BUZZER_PIN);

    while (true) {
        if (is_stable_reading(SENSOR, DEBOUNCE_TIME_MS) && gpio_get(SENSOR) == 1) {
            uint32_t start_time = to_ms_since_boot(get_absolute_time());
            
            while (gpio_get(SENSOR) == 1) {
                beep(BUZZER_PIN, 500);
                gpio_put(LED_R_PIN, 1);
                
                // Verificar se excedeu o tempo máximo de detecção
                if (to_ms_since_boot(get_absolute_time()) - start_time > MAX_DETECTION_TIME_MS) {
                    break;
                }
                
                sleep_ms(100);  // Pequena pausa para não sobrecarregar o sistema
            }
        }

        gpio_put(LED_R_PIN, 0);
        pwm_set_gpio_level(BUZZER_PIN, 0);
        sleep_ms(100);  // Pequena pausa entre verificações
    }

    return 0;
}


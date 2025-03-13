#include "hardware/gpio.h"
#include "pico/stdlib.h"
#include <stdio.h>

const int BTN_PIN_R = 28;
const int LED_PIN_R = 4;

volatile int flag_f_r = 0;       
volatile bool timer_active = false; 
volatile bool led_state = false; 
repeating_timer_t timer;        

bool timer_callback(repeating_timer_t *rt) {
    led_state = !led_state;
    gpio_put(LED_PIN_R, led_state);
    return true; 
}

void btn_callback(uint gpio, uint32_t events) {
    if (events & GPIO_IRQ_EDGE_FALL) {
        flag_f_r = 1;
    }
}

int main() {
    stdio_init_all();

    gpio_init(LED_PIN_R);
    gpio_set_dir(LED_PIN_R, GPIO_OUT);
    gpio_put(LED_PIN_R, 0);

    gpio_init(BTN_PIN_R);
    gpio_set_dir(BTN_PIN_R, GPIO_IN);
    gpio_pull_up(BTN_PIN_R);
    gpio_set_irq_enabled_with_callback(BTN_PIN_R, GPIO_IRQ_EDGE_FALL, true, &btn_callback);

    while (true) {
        if (flag_f_r) {
            flag_f_r = 0; 

            if (!timer_active) {
                if (add_repeating_timer_ms(500, timer_callback, NULL, &timer)) {
                    timer_active = true;
                } else {
                    printf("Falha ao adicionar timer\n");
                }
            } else {
                cancel_repeating_timer(&timer);
                timer_active = false;
                led_state = false;
                gpio_put(LED_PIN_R, 0);
            }
        }
    }
    return 0;
}

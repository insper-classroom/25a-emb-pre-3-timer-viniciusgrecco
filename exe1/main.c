#include "hardware/gpio.h"
#include "pico/stdlib.h"
#include <stdio.h>

const int BTN_PIN_R = 28;
const int LED_PIN_R = 4;

volatile bool button_pressed = false;
volatile bool timer_fired = false;
volatile bool timer_running = false;
repeating_timer_t timer_0;

void btn_callback(uint gpio, uint32_t events) {
    if (events == 0x4) { // Falling edge
        button_pressed = true;
    }
}

bool timer_0_callback(repeating_timer_t *rt) {
    timer_fired = true;
    return true; 
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

   
    int timer_hz = 2;

    while (true) {
        if (button_pressed) {
            button_pressed = false;  
            if (!timer_running) {
                if (add_repeating_timer_us(1000000 / timer_hz, timer_0_callback, NULL, &timer_0)) {
                    timer_running = true;
                } else {
                    printf("Failed to add timer\n");
                }
            } else {
                cancel_repeating_timer(&timer_0);
                timer_running = false;
                gpio_put(LED_PIN_R, 0);
            }
            sleep_ms(50);
        }

        if (timer_running && timer_fired) {
            bool led_state = gpio_get(LED_PIN_R);
            gpio_put(LED_PIN_R, !led_state);
            timer_fired = false; 
        }
    }
    return 0;
}

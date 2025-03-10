#include "hardware/gpio.h"
#include "pico/stdlib.h"
#include <stdio.h>

const int BTN_PIN_R = 28;
const int LED_PIN_R = 4;

volatile int timer_running = 0;
volatile int last_time = 0;

void btn_callback(int gpio, int events) { 
    if (events == GPIO_IRQ_EDGE_FALL) { 
        timer_running = !timer_running;
        if (!timer_running) {
            gpio_put(LED_PIN_R, 0);
        }
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
    gpio_set_irq_enabled_with_callback(BTN_PIN_R, GPIO_IRQ_EDGE_FALL, true, (gpio_irq_callback_t)btn_callback);

    while (1) {
        if (timer_running) {
            int current_time = to_ms_since_boot(get_absolute_time());
            if (current_time - last_time >= 500) { 
                gpio_put(LED_PIN_R, !gpio_get(LED_PIN_R)); 
                last_time = current_time;
            }
        }
    }
}

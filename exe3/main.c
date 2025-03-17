#include "hardware/gpio.h"
#include "pico/stdlib.h"
#include <stdio.h>

const int BTN_PIN_R = 28;
const int LED_PIN_R = 4;

volatile int flag_btn_1 = 0;
volatile int flag_btn_2 = 0;
volatile absolute_time_t apertou;

void btn_callback(uint gpio, uint32_t events) {
    if (events == 0x4) { // fall edge
        if (gpio == BTN_PIN_R){
            flag_btn_1 = 1;
            apertou = get_absolute_time();
        }
    } else if (events == 0x8) { // rise edge
        if (gpio == BTN_PIN_R){
            flag_btn_2 = 1;
        }
    }
}

int main() {
    stdio_init_all();

    gpio_init(LED_PIN_R);
    gpio_set_dir(LED_PIN_R, GPIO_OUT);

    gpio_init(BTN_PIN_R);
    gpio_set_dir(BTN_PIN_R, GPIO_IN);
    gpio_pull_up(BTN_PIN_R);

    gpio_set_irq_enabled_with_callback(
        BTN_PIN_R, GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE, true, &btn_callback);

    while (true) {

        if (flag_btn_2) {
            flag_btn_2 = 0;

            if (flag_btn_1) {
                int soltou = get_absolute_time();
                int diferenca = soltou - apertou;
                
                if (diferenca >= 500000) {
                    gpio_put(LED_PIN_R, !gpio_get(LED_PIN_R));
                }
                flag_btn_1 = 0;
            }
        }
        sleep_ms(10);
    }
}
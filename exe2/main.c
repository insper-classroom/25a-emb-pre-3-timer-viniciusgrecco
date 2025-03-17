#include "hardware/gpio.h"
#include "pico/stdlib.h"
#include <stdio.h>

const int BTN_PIN_R = 28;
const int BTN_PIN_G = 26;

const int LED_PIN_R = 4;
const int LED_PIN_G = 6;

volatile int flag_r_btn = 0;
volatile int flag_g_btn = 0;
volatile int flag_timer_r = 0;
volatile int flag_timer_g = 0;

void btn_callback(uint gpio, uint32_t events) {
    if (events == 0x4) {
        if (gpio == BTN_PIN_R)
            flag_r_btn = 1;
        else if (gpio == BTN_PIN_G)
            flag_g_btn = 1;
    }
}

bool timer_r_callback(repeating_timer_t *rt) {
    flag_timer_r = 1;
    return true;
}

bool timer_g_callback(repeating_timer_t *rt) {
    flag_timer_g = 1;
    return true;
}

int main() {
    gpio_init(LED_PIN_R);
    gpio_set_dir(LED_PIN_R, GPIO_OUT);

    gpio_init(LED_PIN_G);
    gpio_set_dir(LED_PIN_G, GPIO_OUT);

    gpio_init(BTN_PIN_R);
    gpio_set_dir(BTN_PIN_R, GPIO_IN);
    gpio_pull_up(BTN_PIN_R);

    gpio_init(BTN_PIN_G);
    gpio_set_dir(BTN_PIN_G, GPIO_IN);
    gpio_pull_up(BTN_PIN_G);

    gpio_set_irq_enabled_with_callback(BTN_PIN_R, GPIO_IRQ_EDGE_FALL, true, &btn_callback);
    gpio_set_irq_enabled(BTN_PIN_G, GPIO_IRQ_EDGE_FALL, true);

    int timer_r_hz = 2;
    int timer_r_rodando = 0;
    repeating_timer_t timer_r;

    int timer_g_hz = 4;
    int timer_g_rodando = 0;
    repeating_timer_t timer_g;

    while (true) {

        if (flag_r_btn) {
            flag_r_btn = 0;
            if (timer_r_rodando == 0) {
                if (!add_repeating_timer_us(1000000 / timer_r_hz,
                                            timer_r_callback,
                                            NULL, &timer_r)) {
                    printf("Failed to add timer \n");
                } else {
                    timer_r_rodando = 1;
                    printf("Hello Timer Vermelho \n");
                }
            } else {
                cancel_repeating_timer(&timer_r);
                timer_r_rodando = 0;
                gpio_put(LED_PIN_R, 0);
                printf("Timer vermelho cancelado\n");
            }
        }

        if (flag_timer_r) {
            flag_timer_r = 0;
            gpio_put(LED_PIN_R, !gpio_get(LED_PIN_R));
        }
        sleep_ms(50);

        if (flag_g_btn) {
            flag_g_btn = 0;
            if (timer_g_rodando == 0) {
                if (!add_repeating_timer_us(1000000 / timer_g_hz,
                                            timer_g_callback,
                                            NULL, &timer_g)) {
                    printf("Failed to add timer \n");
                } else {
                    timer_g_rodando = 1;
                    printf("Hello Timer Verde \n");
                }
            } else {
                cancel_repeating_timer(&timer_g);
                timer_g_rodando = 0;
                gpio_put(LED_PIN_G, 0);
                printf("Timer verde cancelado\n");
            }
        }

        if (flag_timer_g) {
            flag_timer_g = 0;
            gpio_put(LED_PIN_G, !gpio_get(LED_PIN_G));
        }
        sleep_ms(50);
    }
}

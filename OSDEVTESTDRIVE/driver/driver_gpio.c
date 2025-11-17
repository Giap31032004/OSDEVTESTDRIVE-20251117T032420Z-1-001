#include "driver_gpio.h"

void gpio_config_uart_pins(void) {
    GPIO_PORTA_AFSEL |= (1 << 0) | (1 << 1);
    GPIO_PORTA_DEN   |= (1 << 0) | (1 << 1);
    GPIO_PORTA_PCTL  &= ~0xFF;
    GPIO_PORTA_PCTL  |= 0x11;
}

void gpio_config_led_pin(void) {
    GPIO_PORTA_DIR   |= (1 << 2);
    GPIO_PORTA_DEN   |= (1 << 2);
    GPIO_PORTA_AFSEL &= ~(1 << 2);
}

void gpio_toggle(uint32_t pin) {
    GPIO_PORTA_DATA ^= (1 << pin);
}

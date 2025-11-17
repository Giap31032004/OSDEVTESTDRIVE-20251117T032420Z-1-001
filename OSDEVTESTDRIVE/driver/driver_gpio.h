#ifndef DRIVER_GPIO_H
#define DRIVER_GPIO_H

#include <stdint.h>

#define GPIO_PORTA_BASE  0x40004000
#define GPIO_PORTA_DATA  (*(volatile uint32_t *)(GPIO_PORTA_BASE + 0x3FC))
#define GPIO_PORTA_DIR   (*(volatile uint32_t *)(GPIO_PORTA_BASE + 0x400))
#define GPIO_PORTA_AFSEL (*(volatile uint32_t *)(GPIO_PORTA_BASE + 0x420))
#define GPIO_PORTA_DEN   (*(volatile uint32_t *)(GPIO_PORTA_BASE + 0x51C))
#define GPIO_PORTA_PCTL  (*(volatile uint32_t *)(GPIO_PORTA_BASE + 0x52C))

void gpio_config_uart_pins(void);
void gpio_config_led_pin(void);
void gpio_toggle(uint32_t pin);

#endif

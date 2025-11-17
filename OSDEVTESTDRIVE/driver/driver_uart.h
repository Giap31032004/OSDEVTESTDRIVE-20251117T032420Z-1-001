#ifndef DRIVER_UART_H
#define DRIVER_UART_H
#include<stdint.h>

void uart_init(uint32_t baudrate);
void uart_putc(char c);
void uart_puts(const char* s);
void uart_printf(const char* format, ...);

#endif 


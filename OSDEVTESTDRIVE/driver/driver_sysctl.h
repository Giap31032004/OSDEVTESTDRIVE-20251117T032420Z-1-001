#ifndef DRIVER_SYSCTL_H
#define DRIVER_SYSCTL_H

#include <stdint.h>

// địa chỉ thanh ghi clock control
#define SYSCTL_RCGC1 (*(volatile uint32_t *)0x400FE104)
#define SYSCTL_RCGC2 (*(volatile uint32_t *)0x400FE108)

// các bit bật clock cho từng module
#define SYSCTL_RCGC1_UART0 (1 << 0)
#define SYSCTL_RCGC2_GPIOA (1 << 0)

void sysctl_enable_uart0(void);
void sysctl_enable_gpioa(void);

#endif


#include "driver_sysctl.h"
#include <stdint.h>

void sysctl_enable_uart0(void) {
	SYSCTL_RCGC1 |= SYSCTL_RCGC1_UART0;
	for (volatile int i = 0; i < 100; i++);
}

void sysctl_enable_gpioa(void) {
	SYSCTL_RCGC2 |= SYSCTL_RCGC2_GPIOA;
	for (volatile int i = 0; i < 100; i++);
}


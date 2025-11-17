#include "driver_uart.h"
#include "driver_sysctl.h"
#include "driver_gpio.h"

#include <stdint.h>
#include <string.h>
#include <stdarg.h>

static void itoa_dec(int val, char *buf) {
	char tmp[12];
	int i = 0, j, neg = 0;
	if (val == 0) { buf[0] = '0'; buf[1] = '\0'; return; }
	if (val < 0) { neg = 1; val = -val; }
	while (val > 0) { tmp[i++] = '0' + (val % 10); val /= 10; }
	j = 0;
	if (neg) buf[j++] = '-';
	while (i--) buf[j++] = tmp[i];
	buf[j] = '\0';
}

static void itoa_hex(unsigned int val, char *buf) {
	char tmp[9];
	int i = 0, j;
	if (val == 0) { buf[0] = '0'; buf[1] = '\0'; return; }
	while (val > 0) {
		unsigned int d = val & 0xF;
		tmp[i++] = (d < 10) ? ('0' + d) : ('a' + (d - 10));
		val >>= 4;
	}
	j = 0;
	while (i--) buf[j++] = tmp[i];
	buf[j] = '\0';
}


#define UART0_BASE 0x4000C000
#define UART0_DR   (*(volatile uint32_t *)(UART0_BASE + 0x000))
#define UART0_FR   (*(volatile uint32_t *)(UART0_BASE + 0x018))
#define UART0_IBRD (*(volatile uint32_t *)(UART0_BASE + 0x024))
#define UART0_FBRD (*(volatile uint32_t *)(UART0_BASE + 0x028))
#define UART0_LCRH (*(volatile uint32_t *)(UART0_BASE + 0x02C))
#define UART0_CTL   (*(volatile uint32_t *)(UART0_BASE + 0x030))
#define UART0_CC   (*(volatile uint32_t *)(UART0_BASE + 0xFC8))

#define UART_FR_TXFF (1 << 5) 
#define UART_CTL_UARTEN (1 << 0)
#define UART_CTL_TXE (1 << 8)
#define UART_CTL_RXE (1 << 9)


void uart_init(uint32_t baudrate) {
	// bật clock cho UART0
	sysctl_enable_uart0();
	sysctl_enable_gpioa();

	// cấu hình chân GPIO cho UART0
	gpio_config_uart_pins();
	(void)baudrate; // chưa dùng biến , có thể tính sau
	// tắt uart trước khi cấu hình
	UART0_CTL = 0;
	 
	// cấu hình baudrate
	UART0_IBRD = 8; // giả sử clock là 16MHz, baudrate 115200 Interger part
	UART0_FBRD = 44; // Fractional part

	// cấu hình frame : 8 bit , FIFO bật 
	UART0_LCRH = (0x3 << 5) | (1 << 4); // 8 bits, FIFO enable

	// chọn clock nguồn system 
	UART0_CC = 0x0; // system clock

	// bật UART, TX và RX
	UART0_CTL = UART_CTL_UARTEN | UART_CTL_TXE | UART_CTL_RXE;
}

void uart_putc(char c) {
	// chờ đến khi TX FIFO không đầy
	while (UART0_FR & UART_FR_TXFF);
	// gửi ký tự
	UART0_DR = c;
}

void uart_puts(const char* s) {
	while (*s) {
		uart_putc(*s++); // gửi từng ký tự trong chuỗi
	}
}

void uart_printf(const char* format, ...){
	char out[256];
	const char *p = format;
	char *o = out;
	va_list args;
	va_start(args, format);
	while (*p) {
		if (*p != '%') { *o++ = *p++; continue; }
		p++; /* skip % */
		if (*p == 's') {
			const char *s = va_arg(args, const char*);
			while (*s) *o++ = *s++;
		} else if (*p == 'd') {
			int v = va_arg(args, int);
			char tmp[16]; itoa_dec(v, tmp);
			char *t = tmp; while (*t) *o++ = *t++;
		} else if (*p == 'x') {
			unsigned int v = va_arg(args, unsigned int);
			char tmp[16]; itoa_hex(v, tmp);
			char *t = tmp; while (*t) *o++ = *t++;
		} else if (*p == 'c') {
			char c = (char)va_arg(args, int);
			*o++ = c;
		} else {
			/* unknown specifier, print as-is */
			*o++ = '%'; *o++ = *p;
		}
		p++;
	}
	va_end(args);
	*o = '\0';
	uart_puts(out);
}


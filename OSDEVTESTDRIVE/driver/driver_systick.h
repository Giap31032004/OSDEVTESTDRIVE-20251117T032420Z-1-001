#ifndef DRIVER_SYSTICK_H
#define DRIVER_SYSTICK_H

#include <stdint.h>

#define SYSTICK_BASE   0xE000E010
#define SYSTICK_CTRL   (*(volatile uint32_t *)(SYSTICK_BASE + 0x00))
#define SYSTICK_LOAD   (*(volatile uint32_t *)(SYSTICK_BASE + 0x04))
#define SYSTICK_VAL    (*(volatile uint32_t *)(SYSTICK_BASE + 0x08))
#define SYSTICK_CALIB  (*(volatile uint32_t *)(SYSTICK_BASE + 0x0C))

// Các bit điều khiển trong thanh ghi CTRL
#define SYSTICK_CTRL_ENABLE     (1 << 0)
#define SYSTICK_CTRL_TICKINT    (1 << 1)
#define SYSTICK_CTRL_CLKSOURCE  (1 << 2)
#define SYSTICK_CTRL_COUNTFLAG  (1 << 16)

// Hàm khởi tạo và điều khiển SysTick
void systick_init(uint32_t ticks);
void systick_enable(void);
void systick_disable(void);
void systick_set_reload(uint32_t ticks);
void systick_get_current_value(uint32_t *value);
void systick_handler(void);

#endif

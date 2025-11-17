#include "driver_systick.h"
#include "../os/os.h"

// Hàm khởi tạo SysTick timer với chu kỳ nạp (ticks)
void systick_init(uint32_t ticks) {
    SYSTICK_LOAD = ticks - 1;            // Giá trị reload (chu kỳ ngắt)
    SYSTICK_VAL  = 0;                    // Reset bộ đếm hiện tại
    SYSTICK_CTRL = SYSTICK_CTRL_CLKSOURCE | 
                   SYSTICK_CTRL_TICKINT   | 
                   SYSTICK_CTRL_ENABLE;   // Dùng clock CPU, bật ngắt, bật timer
}

// Bật SysTick (nếu tạm tắt trước đó)
void systick_enable(void) {
    SYSTICK_CTRL |= SYSTICK_CTRL_ENABLE;
}

// Tắt SysTick
void systick_disable(void) {
    SYSTICK_CTRL &= ~SYSTICK_CTRL_ENABLE;
}

// Cập nhật lại chu kỳ reload (thay đổi tốc độ ngắt)
void systick_set_reload(uint32_t ticks) {
    SYSTICK_LOAD = ticks - 1;
}

// Lấy giá trị đếm hiện tại
void systick_get_current_value(uint32_t *value) {
    *value = SYSTICK_VAL;
}

// Trình xử lý ngắt SysTick (gọi từ vector table)
void systick_handler(void) {
    os_tick_handler();  // Gọi hàm xử lý tick của hệ điều hành
    systick_enable(); // Đảm bảo SysTick tiếp tục hoạt động
}

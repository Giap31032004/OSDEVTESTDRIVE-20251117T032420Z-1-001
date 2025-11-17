#include "driver/driver_uart.h"
#include "task_run/task_run.h"
#include "os/os.h"
#include "driver/driver_sysctl.h"
#include "driver/driver_gpio.h"


#include <stdint.h>

static uint32_t stack_sensor[128];
static uint32_t stack_uart[128];
static uint32_t stack_blink[128];
static uint32_t stack_idle[128];

// void task1(void) {
//     while (1) {
//         uart_printf("Task 1 running...\r\n");
//         for (volatile int i = 0; i < 1000000; i++);
//     }
// }

// void task2(void) {
//     while (1) {
//         uart_printf("Task 2 running...\r\n");
//         for (volatile int i = 0; i < 1000000; i++);
//     }
// }


int main(void) {
    uart_init(115200);
    gpio_config_led_pin();

    uart_puts("Hello, Cortex-M3 UART!\r\n");
    uart_puts("Hello, Cortex-M2 UART!\r\n");


    os_init();

    // os_create_process(task1, &stack_sensor[127], 1);
    // os_create_process(task2, &stack_uart[127], 2);

    os_create_process(task_sensor, &stack_sensor[127], 1);
    os_create_process(task_uart, &stack_uart[127], 2);
    os_create_process(task_blink, &stack_blink[127], 3);
    os_create_process(idle_process, &stack_idle[127], 0);

    os_start();
    
    while (1);
}

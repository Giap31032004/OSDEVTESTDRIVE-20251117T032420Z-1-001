#include <stdint.h>
#include "task_run.h"
#include "driver_uart.h"
#include "driver_gpio.h"
#include "driver_sysctl.h"
#include "../os/os.h"

void task_sensor(void) {
    static int sensor_value = 0;
    while (1) {
        sensor_value = (sensor_value + 1) % 100;  // giả lập đọc cảm biến
        uart_printf("Sensor value: %d\r\n", sensor_value);
        os_delay(500);  // delay 500ms
    }
}

void task_uart(void) {
    while (1) {
        uart_puts("UART task alive\r\n");
        os_delay(1000);  // delay 1s
    }
}

void task_blink(void) {
    gpio_config_led_pin(); 

    while (1) {
        gpio_toggle(2);     
        os_delay(500);
    }
}

// void task_mutex_test_1(void){
//     while(1){
//         mutex_lock(&uart1_mutex);
//         uart_puts("Mutex test task acquired the mutex\r\n");
//         mutex_unlock(&uart1_mutex);
//         os_delay(1000);
//     }
// }

// void task_mutex_test_2(void){
//     while(1){
//         mutex_lock(&uart2_mutex);
//         uart_puts("Mutex test 2 task acquired the mutex\r\n");
//         mutex_unlock(&uart2_mutex);
//         os_delay(1000);
//     }
// }

// void sensor_ISR(void){
//     sem_signal(&sensor_ready);
// }

// void task_sensor(void){
//     while(1){
//         semaphore_wait(&sensor_ready);
//         uart_printf("Sensor data ready\r\n");
//     }
// }

// void task_producer(void){
//     while(1){
//         os_delay(1000);
//         sensor_ISR();
//     }
// }



#ifndef TASK_RUN_H
#define TASK_RUN_H
#include <stdint.h>
#include "../os/os.h"

extern mutex_p uart1_mutex;
extern mutex_p uart2_mutex;
extern semaphore_p sensor_ready;

void task_sensor(void);
void task_uart(void);
void task_blink(void);
void task_mutex_test_1(void);
void task_mutex_test_2(void);


#endif
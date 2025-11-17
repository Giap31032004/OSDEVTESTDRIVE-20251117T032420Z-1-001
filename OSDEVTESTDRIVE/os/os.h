#ifndef OS_H
#define OS_H

#include <stdint.h>

#define  MAX_TASKS 8 // so luong tien trinh toi da
#define MAX_PRIORITY 10 // so uu tien toi da
#define MAX_CHILDREN 5 // so luong con toi da cua mot tien trinh
#define  MAILBOX_SIZE 10
#define MSG_SIZE 32

typedef enum {
    TASK_READY,
    TASK_RUNNING,
    TASK_WAITING,
    TASK_SLEEPING,
    TASK_SUSPENDED,
    TASK_ERROR,
    TASK_TERMINATED
} process_state_t;

typedef enum{
    IO_TYPE_UART,
    IO_TYPE_GPIO,
    IO_TYPE_ADC,
    IO_TYPE_DAC,
    IO_TYPE_SPI,
    IO_TYPE_I2C,
    UNKNOWN_IO
} type_io;

typedef struct {
    int count_io; // so luong thiet bi
    type_io type_io; // loai thiet bi
    uint8_t id; // so hieu thiet bi
    void* hw_registers; // con tro den thanh ghi phan cung
    uint8_t interrupt_number; // so ngat cua thiet bi
    uint8_t flags; // trang thai cua thiet bi
    void* buffer; // bo dem du lieu
    uint32_t buffer_size; // kich thuoc bo dem

} io_device_p;

typedef struct PCB {
    volatile uint32_t *stack_pointer;
    process_state_t state;
    uint8_t id;
    void (*process_func)(void);

    uint8_t priority; // do uu tien cua tien trinh
    uint8_t dynamic_priority; // do uu tien dong
    volatile uint32_t cpu_time; // thoi gian CPU da su dung

    uint32_t start_time; // thoi diem bat dau
    volatile uint32_t elapsed_time; // thoi gian da chay

    io_device_p *io_devices; // con tro den cac thiet bi vao/ra
    uint8_t io_count; // so luong thiet bi

    uint32_t wake_tick; // tick de thuc day tien trinh tu trang thai ngu

    struct PCB* parent; // con tro den tien trinh cha
    struct PCB* child_list[MAX_CHILDREN]; // danh sach con
    int child_count; // so luong con
} PCB;

typedef struct 
{
    PCB* process[MAX_TASKS];
    int head;
    int tail;
    int count;
}queue;

typedef struct{
    uint8_t locked;
    PCB* owner;
}mutex_p;

typedef struct{
    int count;

}semaphore_p;

typedef struct{
    char data[MSG_SIZE];
}Message;

typedef struct{
    Message buffer[MAILBOX_SIZE];
    int head;
    int tail;
    int count;
    mutex_p mutex;
    semaphore_p full;
    semaphore_p empty;
}mailbox_p;

void queue_init(queue* q);
int enqueue(queue* q, PCB* process);
PCB* dequeue(queue* q);

void os_init(void);
void os_start(void);
void os_create_process(void (*process_function)(void), uint32_t *stack_top, uint8_t priority);
void os_delay(uint32_t ticks);
void os_tick_handler(void);
void check_sleeping_processes(void);
void os_scheduler(void);
void start_first_process(volatile uint32_t *stack_pointer);
void aging_process(void);
void switch_process(volatile uint32_t *prev_sp, volatile uint32_t *new_sp);
PCB* select_next_process(void);
int remove_from_queue(queue* q, PCB* process);
// idle process
void idle_process(void);
// mutex va semaphore
void mutex_init(mutex_p* m);
void mutex_lock(mutex_p* m);
void mutex_unlock(mutex_p* m);
void semaphore_init(semaphore_p* s, int initial_count);
void semaphore_wait(semaphore_p* s);
void semaphore_signal(semaphore_p* s);
void semaphore_signal(semaphore_p* s);
// huy process
void os_terminate_process(PCB* process);
// khoi tao mailbox
void mailbox_init(mailbox_p* m);
void mailbox_send(mailbox_p* m, Message* msg);
void mailbox_receive(mailbox_p* m, Message* msg);

extern queue* ready_queue;
extern queue* IO_wait_queue;
extern queue* job_queue;
extern queue* sleep_queue;
#endif

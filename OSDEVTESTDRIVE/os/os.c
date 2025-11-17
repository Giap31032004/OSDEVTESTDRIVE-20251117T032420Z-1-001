#include "os.h"
#include "driver_systick.h"

#include<stddef.h>
#include <stdint.h>

extern void switch_context(uint32_t **prev_sp, uint32_t **next_sp);
extern void start_first_task(uint32_t *stack_pointer);

// Global OS data structures

queue ready_queue_storage;
queue* ready_queue = &ready_queue_storage;

queue IO_wait_queue_storage;
queue* IO_wait_queue = &IO_wait_queue_storage;

queue job_queue_storage;
queue* job_queue = &job_queue_storage;

queue sleep_queue_storage;
queue* sleep_queue = &sleep_queue_storage;

PCB process[MAX_TASKS];
int process_count = 0;
PCB* current_process = NULL;

// khoi tao ham queue, enqueue, dequeue

void queue_init(queue* q){
    q -> head = 0;
    q -> tail = 0;
    q -> count = 0;
}

int enqueue(queue*q, PCB* process){
    if(q -> count < MAX_TASKS){
        q -> process[q -> tail] = process;
        q -> tail = (q -> tail + 1) % MAX_TASKS;
        q -> count++;
        return 0;
    }
    return -1;
}

PCB* dequeue(queue* q){
    if(q->count == 0) return NULL; 
    PCB* p = q->process[q->head];
    q->head = (q->head + 1) % MAX_TASKS;
    q->count--;
    return p;
}


static volatile uint32_t os_tick_count =0;
void os_tick_handler(void)
{
    os_tick_count++;
    check_sleeping_processes();
    aging_process();
    os_scheduler();
}

void aging_process(void){
    int n = ready_queue -> count;
    for(int i = 0; i < n; i++){
        PCB* p = dequeue(ready_queue);
        if(p -> dynamic_priority < MAX_PRIORITY){
            p -> dynamic_priority++;
        }
        enqueue(ready_queue, p);
    }
}

void check_sleeping_processes(void){
    int n = sleep_queue -> count;
    for(int i = 0; i < n; i ++){
        PCB* p = dequeue(sleep_queue);
        if(os_tick_count >= p -> wake_tick){
            p -> state = TASK_READY;
            enqueue(ready_queue, p);
        } else {
            enqueue(sleep_queue, p);
        }
    }
}

void os_delay(uint32_t ticks){
    current_process -> wake_tick = os_tick_count +ticks;
    current_process -> state = TASK_SLEEPING;
    enqueue(sleep_queue, current_process);
    os_scheduler();
}

void os_start(){
    systick_init(16000);
    current_process = dequeue(ready_queue);
    if(current_process){
        start_first_task(current_process -> stack_pointer);
    }
}

void os_init(void){
    queue_init(ready_queue);
    queue_init(IO_wait_queue);
    queue_init(job_queue);
    queue_init(sleep_queue);
    process_count =0;
    current_process = NULL;
    os_tick_count =0;
}

void os_create_process(void(*process_func)(void), uint32_t* stack_top, uint8_t priority){
    if(process_count >= MAX_TASKS) return;

    uint32_t* sp = stack_top;

    // ===== QUAN TRỌNG: Stack frame theo đúng thứ tự ARM =====
    
    // Hardware-saved context (8 registers)
    *(--sp) = 0x01000000;           // xPSR (Thumb bit set)
    *(--sp) = (uint32_t)process_func; // PC (entry point)
    *(--sp) = 0xFFFFFFFD;           // LR (EXC_RETURN - return to Thread mode with PSP)
    *(--sp) = 0x12121212;           // R12
    *(--sp) = 0x03030303;           // R3
    *(--sp) = 0x02020202;           // R2
    *(--sp) = 0x01010101;           // R1
    *(--sp) = 0x00000000;           // R0
    
    // Software-saved context (8 registers)
    *(--sp) = 0x11111111;           // R11
    *(--sp) = 0x10101010;           // R10
    *(--sp) = 0x09090909;           // R9
    *(--sp) = 0x08080808;           // R8
    *(--sp) = 0x07070707;           // R7
    *(--sp) = 0x06060606;           // R6
    *(--sp) = 0x05050505;           // R5
    *(--sp) = 0x04040404;           // R4

    PCB* new_process = &process[process_count++];
    new_process->stack_pointer = sp;  // Lưu stack pointer sau khi init
    new_process->state = TASK_READY;
    new_process->id = process_count - 1;
    new_process->priority = priority;
    new_process->dynamic_priority = priority;
    new_process->cpu_time = 0;
    new_process->start_time = 0;
    new_process->elapsed_time = 0;
    new_process->io_devices = NULL;
    new_process->io_count = 0;
    
    // ===== FIX: LƯU FUNCTION POINTER =====
    new_process->process_func = process_func;

    // Thiết lập tiến trình cha con
    new_process->parent = current_process;
    new_process->child_count = 0;
    if(current_process && current_process->child_count < MAX_CHILDREN){
        current_process->child_list[current_process->child_count++] = new_process;
    }

    enqueue(ready_queue, new_process);
}

void os_scheduler(void){
    PCB* prev_process = current_process;

    // Nếu task hiện tại đang chạy, chuyển về trạng thái READY
    if(current_process && current_process->state == TASK_RUNNING){
        current_process->state = TASK_READY;
        enqueue(ready_queue, current_process);
    }

    // Chọn task tiếp theo
    PCB* next_process = select_next_process();

    if(next_process){
        current_process = next_process;
        current_process->state = TASK_RUNNING;
        
        if(prev_process != NULL && prev_process != next_process){
            // Switch context giữa 2 tasks
            switch_context(
                (uint32_t**)&prev_process->stack_pointer, 
                (uint32_t**)&next_process->stack_pointer
            );
        }
        // Nếu prev_process == NULL, nghĩa là đang ở main, không cần switch
        // Nếu prev_process == next_process, cùng task, không cần switch
    }
}

// void switch_process(volatile uint32_t *prev_sp, volatile uint32_t *new_sp) {
//     (void)prev_sp; (void)new_sp;
//     if (current_process && current_process->process_func) {
//         current_process->process_func();
//     }
// }

PCB* select_next_process(void){
    PCB* select = NULL;
    int index = ready_queue -> head;
    for(int i = 0; i < ready_queue -> count; i++){
        PCB* p = ready_queue -> process[index];
        if(select == NULL || p -> dynamic_priority > select -> dynamic_priority){
            select = p;
        }
        index = (index + 1)% MAX_TASKS;
    }
    remove_from_queue(ready_queue, select);
    return select;
}

int remove_from_queue(queue* q, PCB* process){
    if(q -> count == 0) return -1; // queue rong

    int found = -1;

    for(int i = 0; i < q->count; i++){
        int idx = (q->head + i) % MAX_TASKS;
        if(q->process[idx] == process){
            found = idx;
            break;
        }
    }

    if(found == -1) return -2; // khong tim thay

    for(int i = 0; i < q->count - 1; i++){
        int cur = (found + i) % MAX_TASKS;
        int next = (found + i + 1) % MAX_TASKS;
        q->process[cur] = q->process[next];
    }

    q->tail = (q->tail + MAX_TASKS - 1) % MAX_TASKS;
    q->count--;
    return 0;
}

void idle_process(void){
    while(1) {
        __asm__ volatile ("wfi");
    }
}

// Mutex and Semaphore functions
void mutex_init(mutex_p* m){
    m -> locked =0;
    m -> owner = NULL;
}

void mutex_lock(mutex_p*m){
    while(m -> locked && m->owner != current_process){
        os_scheduler();
    }
    m->locked = 1;
    m->owner = current_process;
}

void mutex_unlock(mutex_p* m){
    if(m->owner == current_process){
        m->locked =0;
        m->owner = NULL;
    }
}

void semaphore_init(semaphore_p* s, int initial_count){
    s -> count = initial_count;
}

void semaphore_wait(semaphore_p* s){
    while(s->count <= 0){
        os_scheduler();
    }
    s->count--;
}

void semaphore_signal(semaphore_p* s){
    s->count++;
}

// huy process
void os_terminate_process(PCB* process){
    if(!process) return;

    // huy cac tien trinh con
    for(int i =0; i < process -> child_count; i++){
        os_terminate_process(process -> child_list[i]);
    }

    // doi trang thai
    process -> state = TASK_TERMINATED;

    // xoa khoi cac queue
    remove_from_queue(ready_queue, process);
    remove_from_queue(sleep_queue, process);
    remove_from_queue(IO_wait_queue, process);
    remove_from_queue(job_queue, process);

    // giai phong khoi stack
    process -> stack_pointer = NULL;

    // neu dang chay thi chuyen tien trinh
    if(process == current_process){
        os_scheduler();
    }
}

// mailbox 
void mailbox_init(mailbox_p* m){
    m->head = 0;
    m->tail = 0;
    m->count = 0;
    mutex_init(&m->mutex);
    semaphore_init(&m->full, 0);
    semaphore_init(&m->empty, MAILBOX_SIZE); 
}

void mailbox_send(mailbox_p* m, Message* msg){
    semaphore_wait(&m->empty);
    mutex_lock(&m->mutex);

    // gui tin nhan
    m->buffer[m->tail] = *msg;
    m->tail = (m->tail + 1) % MAILBOX_SIZE;
    m->count++;

    mutex_unlock(&m->mutex);
    semaphore_signal(&m->full);
}

void mailbox_receive(mailbox_p* m, Message* msg){
    semaphore_wait(&m->full);
    mutex_lock(&m->mutex);

    // nhan tin nhan
    *msg = m->buffer[m->head];
    m->head = (m->head + 1) % MAILBOX_SIZE;
    m->count--;

    mutex_unlock(&m->mutex);
    semaphore_signal(&m->empty);
}
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
        
        // ===== FIX: Thực hiện context switch THỰC SỰ =====
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
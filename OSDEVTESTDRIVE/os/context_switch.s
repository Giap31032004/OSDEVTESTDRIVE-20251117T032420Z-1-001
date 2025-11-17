.syntax unified
.cpu cortex-m3
.thumb

.global switch_context
.type switch_context, %function

/*
 * void switch_context(uint32_t **prev_sp, uint32_t **next_sp)
 * 
 * Parameters:
 *   r0 = address of prev_sp (double pointer)
 *   r1 = address of next_sp (double pointer)
 */
switch_context:
    /* ===== SAVE CONTEXT OF CURRENT TASK ===== */
    
    /* Disable interrupts để đảm bảo atomic operation */
    cpsid i
    
    /* Lấy PSP hiện tại (Process Stack Pointer) */
    mrs r2, psp
    
    /* Kiểm tra nếu prev_sp != NULL (task đầu tiên thì NULL) */
    cbz r0, restore_context
    
    /* Lưu R4-R11 vào stack của task cũ */
    stmdb r2!, {r4-r11}     /* r2 = r2 - 32, sau đó lưu R4-R11 */
    
    /* Lưu stack pointer mới vào *prev_sp */
    ldr r3, [r0]            /* r3 = *prev_sp (địa chỉ biến sp) */
    str r2, [r3]            /* **prev_sp = r2 (lưu giá trị sp mới) */
    
restore_context:
    /* ===== RESTORE CONTEXT OF NEW TASK ===== */
    
    /* Load stack pointer của task mới */
    ldr r3, [r1]            /* r3 = *next_sp */
    ldr r2, [r3]            /* r2 = **next_sp (giá trị sp) */
    
    /* Restore R4-R11 từ stack của task mới */
    ldmia r2!, {r4-r11}     /* Load R4-R11, r2 = r2 + 32 */
    
    /* Cập nhật PSP với stack pointer mới */
    msr psp, r2
    
    /* Enable interrupts */
    cpsie i
    
    /* Return về task mới */
    /* Hardware sẽ tự động restore R0-R3, R12, LR, PC, xPSR */
    bx lr

.size switch_context, .-switch_context


/*
 * void start_first_task(uint32_t *stack_pointer)
 * 
 * Khởi động task đầu tiên
 */
.global start_first_task
.type start_first_task, %function

start_first_task:
    /* Load stack pointer vào PSP */
    msr psp, r0
    
    /* Switch sang PSP mode (Thread mode với PSP) */
    mov r0, #2              /* CONTROL = 0b10 (use PSP) */
    msr control, r0
    isb                     /* Instruction Sync Barrier */
    
    /* Pop R4-R11 (software context) */
    ldmia sp!, {r4-r11}
    
    /* Pop R0-R3, R12 */
    ldmia sp!, {r0-r3, r12}
    
    /* Skip LR, pop PC và xPSR */
    add sp, sp, #4          /* Skip LR */
    ldmia sp!, {lr}         /* Load PC into LR */
    add sp, sp, #4          /* Skip xPSR */
    
    /* Branch to first task */
    bx lr

.size start_first_task, .-start_first_task
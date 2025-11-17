.syntax unified
.cpu cortex-m3
.thumb

/* ================== VECTOR TABLE ================== */
.section .isr_vector, "a", %progbits
.global _estack
.global Reset_Handler

vector_table:
    .word _estack             /* Initial Stack Pointer */
    .word Reset_Handler       /* Reset Handler */
    .word Default_Handler     /* NMI */
    .word Default_Handler     /* HardFault */
    .word Default_Handler     /* MemManage */
    .word Default_Handler     /* BusFault */
    .word Default_Handler     /* UsageFault */
    .word 0,0,0,0            /* Reserved */
    .word Default_Handler     /* SVC */
    .word Default_Handler     /* DebugMon */
    .word 0
    .word Default_Handler     /* PendSV */
    .word SysTick_Handler     /* SysTick Handler */

    /* IRQs (chưa dùng) */
    .rept 32
        .word Default_Handler
    .endr

/* ================== RESET HANDLER ================== */
.text
.thumb_func
.global Reset_Handler
Reset_Handler:
    /* 1. Init MSP */
    ldr sp, =_estack

    /* 2. Init .data */
    ldr r0, =_sdata
    ldr r1, =_edata
    ldr r2, =_ldata
copy_data_loop:
    cmp r0, r1        /* while (r0 < r1) */
    bhs copy_data_done
    ldr r3, [r2], #4
    str r3, [r0], #4
    b copy_data_loop
copy_data_done:

    /* 3. Zero .bss */
    ldr r0, =_sbss
    ldr r1, =_ebss
    mov r3, #0
zero_bss_loop:
    cmp r0, r1
    bhs zero_bss_done
    str r3, [r0], #4
    b zero_bss_loop
zero_bss_done:

    /* 4. Call main */
    bl main

    /* 5. Infinite loop if main returns */
    b .

/* ================== DEFAULT HANDLER ================== */
.thumb_func
Default_Handler:
    b .

/* Weak aliases for handlers that may not be implemented by the application. */
.weak SysTick_Handler
.set SysTick_Handler, Default_Handler


#pragma once

#include "../registers.cpp"
#include "fault.cpp"

#include "../../debug/debug.cpp"

extern "C" void universal_fault_handler(uint64_t int_no, uint64_t err_code) {
    //s0::put("void universal_fault_handler()\n\r");
    //s0::put("KERNEL PANIC: ");
    s0::put(exception::messages[int_no]);
    s0::put(" #");
    s0::puthex(int_no);
    s0::put("\n\r");


    Console console;
    console.clear();
    console.write(int_no);
    //stop();

    registers_t regs = {};
    get_basic_registers(&regs);

    regs.int_no = int_no;
    regs.err_code = err_code;

    uint64_t current_rsp;
    asm volatile ("mov %%rsp, %0" : "=r" (current_rsp));
    regs.excesp = current_rsp;

    uint64_t *stack = (uint64_t *) current_rsp;

    bool has_error_code = (int_no == 8 || (int_no >= 10 && int_no <= 14) || int_no == 17 || int_no == 21);

    if (has_error_code) {
        regs.rip = stack[1];
        regs.cs = stack[2] & 0xFFFF;
        regs.rflags = stack[3];
        regs.rsp = stack[4];
    } else {
        regs.rip = stack[0];
        regs.cs = stack[1] & 0xFFFF;
        regs.rflags = stack[2];
        regs.rsp = stack[3];
    }

    fault_handler(&regs);

    while (1) {
        asm volatile("hlt");
    }
}

extern "C" void isr_common_stub();


#define DECLARE_ISR_NOERRCODE(num) \
    extern "C" void isr##num() { \
        universal_fault_handler(num, 0);\
    }

#define DECLARE_ISR_ERRCODE(num) \
    extern "C" void isr##num() { \
        universal_fault_handler(num, 0);\
    }


DECLARE_ISR_NOERRCODE(0)
DECLARE_ISR_NOERRCODE(1)
DECLARE_ISR_NOERRCODE(2)
DECLARE_ISR_NOERRCODE(3)
DECLARE_ISR_NOERRCODE(4)
DECLARE_ISR_NOERRCODE(5)
DECLARE_ISR_NOERRCODE(6)
DECLARE_ISR_NOERRCODE(7)
DECLARE_ISR_ERRCODE(8)
DECLARE_ISR_NOERRCODE(9)
DECLARE_ISR_ERRCODE(10)
DECLARE_ISR_ERRCODE(11)
DECLARE_ISR_ERRCODE(12)
DECLARE_ISR_ERRCODE(13)
DECLARE_ISR_ERRCODE(14)
DECLARE_ISR_NOERRCODE(15)
DECLARE_ISR_NOERRCODE(16)
DECLARE_ISR_ERRCODE(17)
DECLARE_ISR_NOERRCODE(18)
DECLARE_ISR_NOERRCODE(19)
DECLARE_ISR_NOERRCODE(20)
DECLARE_ISR_NOERRCODE(21)
DECLARE_ISR_NOERRCODE(22)
DECLARE_ISR_NOERRCODE(23)
DECLARE_ISR_NOERRCODE(24)
DECLARE_ISR_NOERRCODE(25)
DECLARE_ISR_NOERRCODE(26)
DECLARE_ISR_NOERRCODE(27)
DECLARE_ISR_NOERRCODE(28)
DECLARE_ISR_NOERRCODE(29)
DECLARE_ISR_NOERRCODE(30)
DECLARE_ISR_NOERRCODE(31)

/* Или более простой вариант без сохранения сегментов: */

extern "C" void isr_common_simple() {
    asm volatile (
        /* Save RAX temporarily */
            "pushq %rax\n\t"

            /* Load kernel data segments */
            "movw $0x10, %ax\n\t"
            "movw %ax, %ds\n\t"
            "movw %ax, %es\n\t"
            "movw %ax, %fs\n\t"
            "movw %ax, %gs\n\t"

            /* Restore RAX */
            "popq %rax\n\t"

            /* Call handler - parameters already on stack */
            "call universal_fault_handler\n\t"

            /* We shouldn't return from universal_fault_handler */
            "iretq"
            );
}
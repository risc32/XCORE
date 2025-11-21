#pragma once

#include "../registers.cpp"
#include "fault.cpp"


extern "C" void universal_fault_handler(uint32_t int_no, uint32_t err_code) {
    registers_t regs = {};

    get_basic_registers(&regs);

    regs.int_no = err_code;
    regs.err_code = err_code;

    uint32_t current_esp;
    asm volatile ("mov %%esp, %0" : "=r" (current_esp));
    regs.excesp = current_esp;


    uint32_t *stack = (uint32_t *) current_esp;

    bool has_error_code = (int_no == 8 || (int_no >= 10 && int_no <= 14) || int_no == 17);

    if (has_error_code) {
        regs.eip = stack[1];
        regs.cs = stack[2];
        regs.eflags = stack[3];
    } else {
        regs.eip = stack[0];
        regs.cs = stack[1];
        regs.eflags = stack[2];
    }

    fault_handler(&regs);

    while (1) {
        asm volatile("hlt");
    }
}


#define ISR_NO_ERROR(num) \
extern "C" void isr##num() {  \
 \
    asm volatile ( \
        "pushl $0\n\t"           /* фиктивный код ошибки */ \
        "pushl $" #num "\n\t"     /* номер исключения */ \
        "jmp isr_common" \
    ); \
}

#define ISR_ERROR(num) \
extern "C" void isr##num() { \
 \
    asm volatile ( \
        "pushl $" #num "\n\t"     /* номер исключения (код ошибки уже на стеке) */ \
        "jmp isr_common" \
    ); \
}

extern "C" void isr_common() {  ///for stacktrace

    asm volatile (
            "pusha\n\t"
            "pushl %ds\n\t"
            "pushl %es\n\t"
            "pushl %fs\n\t"
            "pushl %gs\n\t"

            "movw $0x10, %ax\n\t"
            "movw %ax, %ds\n\t"
            "movw %ax, %es\n\t"
            "movw %ax, %fs\n\t"
            "movw %ax, %gs\n\t"

            "pushl %esp\n\t"                     "call universal_fault_handler\n\t"
            "addl $4, %esp\n\t"
            "popl %gs\n\t"
            "popl %fs\n\t"
            "popl %es\n\t"
            "popl %ds\n\t"
            "popa\n\t"

            "addl $8, %esp\n\t"

            "iret"
            );
}

ISR_NO_ERROR(0)
ISR_NO_ERROR(1)
ISR_NO_ERROR(2)
ISR_NO_ERROR(3)
ISR_NO_ERROR(4)
ISR_NO_ERROR(5)
ISR_NO_ERROR(6)
ISR_NO_ERROR(7)
ISR_NO_ERROR(9)
ISR_NO_ERROR(15)
ISR_NO_ERROR(16)
ISR_NO_ERROR(18)
ISR_NO_ERROR(19)
ISR_NO_ERROR(20)
ISR_NO_ERROR(21)
ISR_NO_ERROR(22)
ISR_NO_ERROR(23)
ISR_NO_ERROR(24)
ISR_NO_ERROR(25)
ISR_NO_ERROR(26)
ISR_NO_ERROR(27)
ISR_NO_ERROR(28)
ISR_NO_ERROR(29)
ISR_NO_ERROR(30)
ISR_NO_ERROR(31)

ISR_ERROR(8)
ISR_ERROR(10)
ISR_ERROR(11)
ISR_ERROR(12)
ISR_ERROR(13)
ISR_ERROR(14)
ISR_ERROR(17)
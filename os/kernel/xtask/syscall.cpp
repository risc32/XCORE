#pragma once

#include "../CPU/cpu.cpp"
#include "scalls/scalls.cpp"

struct Syscall {
__attribute__((naked))
static void entry(void) {
        INTEL(
        "swapgs\n\t"
        "mov rax, 0x12345678\n\t"
        "swapgs\n\t"
        "sysretq\n\t"
    );
    INTEL(

        "swapgs\n\t"
        "mov [gs:0x10], rsp\n\t"
        "mov rsp, 0x170000\n\t"


        "push rbp\n\t"
        "mov rbp, rsp\n\t"
        "sub rsp, 16\n\t"


        "push rcx\n\t"
        "push r11\n\t"


        "push rax\n\t"
        "push rdx\n\t"
        "push rsi\n\t"
        "push rdi\n\t"
        "push r8\n\t"
        "push r9\n\t"
        "push r10\n\t"
        "push rbx\n\t"
        "push r12\n\t"
        "push r13\n\t"
        "push r14\n\t"
        "push r15\n\t"


        "mov ax, 0x10\n\t"
        "mov ds, ax\n\t"
        "mov es, ax\n\t"



        "mov rdi, rsp\n\t"


        "call syscall_handler\n\t"




        "pop r15\n\t"
        "pop r14\n\t"
        "pop r13\n\t"
        "pop r12\n\t"
        "pop rbx\n\t"
        "pop r10\n\t"
        "pop r9\n\t"
        "pop r8\n\t"
        "pop rdi\n\t"
        "pop rsi\n\t"
        "pop rdx\n\t"
        "pop rax\n\t"


        "pop r11\n\t"
        "pop rcx\n\t"


        "mov rsp, rbp\n\t"
        "pop rbp\n\t"
        "swapgs\n\t"
        "mov rsp, [gs:0x10]\n\t"


        "sysretq\n\t"

        : : : "memory", "cc"
    );
}
    static void init(void) {
        wrmsr(MSR_LSTAR, (uint64_t)entry);
        wrmsr(MSR_STAR, ((uint64_t)0x08 << 32) | ((uint64_t)0x10 << 48));
        wrmsr(MSR_FMASK, 0x00);
        uint64_t efer = rdmsr(MSR_EFER);
        efer |= EFER_SCE;
        wrmsr(MSR_EFER, efer);
    }
};

extern "C" uint64_t syscall_handler(registers_t *regs) {
    uint64_t nr = regs->rax;

    if (nr >= sizeof scalls / sizeof &_hnSyscall::tester) {
        return -ENOSYS;
    }

    get_all_registers(regs);


    return scalls[nr](regs);
}
#pragma once

#include "../types/scalar.cpp"

struct registers_t {
    uint64_t rax, rbx, rcx, rdx;
    uint64_t rsi, rdi, rbp, rsp;
    uint64_t r8, r9, r10, r11;
    uint64_t r12, r13, r14, r15;

    uint16_t cs, ds, es, fs, gs, ss;

    uint64_t rip, rflags;

    uint64_t cr0, cr2, cr3, cr4;

    uint64_t int_no, err_code;

    uint64_t excesp;
};

extern "C" void get_all_registers(registers_t *regs) {
    if (!regs) return;

    asm volatile ("mov %%rax, %0" : "=r" (regs->rax));
    asm volatile ("mov %%rbx, %0" : "=r" (regs->rbx));
    asm volatile ("mov %%rcx, %0" : "=r" (regs->rcx));
    asm volatile ("mov %%rdx, %0" : "=r" (regs->rdx));
    asm volatile ("mov %%rsi, %0" : "=r" (regs->rsi));
    asm volatile ("mov %%rdi, %0" : "=r" (regs->rdi));
    asm volatile ("mov %%rbp, %0" : "=r" (regs->rbp));
    asm volatile ("mov %%rsp, %0" : "=r" (regs->rsp));

    asm volatile ("mov %%r8, %0" : "=r" (regs->r8));
    asm volatile ("mov %%r9, %0" : "=r" (regs->r9));
    asm volatile ("mov %%r10, %0" : "=r" (regs->r10));
    asm volatile ("mov %%r11, %0" : "=r" (regs->r11));
    asm volatile ("mov %%r12, %0" : "=r" (regs->r12));
    asm volatile ("mov %%r13, %0" : "=r" (regs->r13));
    asm volatile ("mov %%r14, %0" : "=r" (regs->r14));
    asm volatile ("mov %%r15, %0" : "=r" (regs->r15));

    asm volatile ("mov %%cs, %0" : "=r" (regs->cs));
    asm volatile ("mov %%ds, %0" : "=r" (regs->ds));
    asm volatile ("mov %%es, %0" : "=r" (regs->es));
    asm volatile ("mov %%fs, %0" : "=r" (regs->fs));
    asm volatile ("mov %%gs, %0" : "=r" (regs->gs));
    asm volatile ("mov %%ss, %0" : "=r" (regs->ss));


    uint64_t rip_temp;
    asm volatile (
            "lea (%%rip), %0\n\t"
            : "=r" (rip_temp)
            );
    regs->rip = rip_temp;

    asm volatile ("pushfq\n\tpop %0" : "=r" (regs->rflags));

    asm volatile ("mov %%cr0, %0" : "=r" (regs->cr0));
    asm volatile ("mov %%cr2, %0" : "=r" (regs->cr2));
    asm volatile ("mov %%cr3, %0" : "=r" (regs->cr3));
    asm volatile ("mov %%cr4, %0" : "=r" (regs->cr4));
}

extern "C" void get_basic_registers(registers_t *regs) {
    if (!regs) return;

    asm volatile ("mov %%rax, %0" : "=r" (regs->rax));
    asm volatile ("mov %%rbx, %0" : "=r" (regs->rbx));
    asm volatile ("mov %%rcx, %0" : "=r" (regs->rcx));
    asm volatile ("mov %%rdx, %0" : "=r" (regs->rdx));
    asm volatile ("mov %%rsi, %0" : "=r" (regs->rsi));
    asm volatile ("mov %%rdi, %0" : "=r" (regs->rdi));
    asm volatile ("mov %%rbp, %0" : "=r" (regs->rbp));
    asm volatile ("mov %%rsp, %0" : "=r" (regs->rsp));

    asm volatile ("mov %%cs, %0" : "=r" (regs->cs));
    asm volatile ("mov %%ds, %0" : "=r" (regs->ds));


    uint64_t rip_temp;
    asm volatile (
            "lea (%%rip), %0\n\t"
            : "=r" (rip_temp)
            );
    regs->rip = rip_temp;

    asm volatile ("pushfq\n\tpop %0" : "=r" (regs->rflags));
    asm volatile ("mov %%cr2, %0" : "=r" (regs->cr2));
}

extern "C" void get_registers_safe(registers_t *regs) {
    if (!regs) return;

    asm volatile (
            "movq %%rax, 0(%0)\n\t"
            "movq %%rbx, 8(%0)\n\t"
            "movq %%rcx, 16(%0)\n\t"
            "movq %%rdx, 24(%0)\n\t"
            "movq %%rsi, 32(%0)\n\t"
            "movq %%rdi, 40(%0)\n\t"
            "movq %%rbp, 48(%0)\n\t"
            "movq %%rsp, 56(%0)\n\t"
            "movq %%r8, 64(%0)\n\t"
            "movq %%r9, 72(%0)\n\t"
            "movq %%r10, 80(%0)\n\t"
            "movq %%r11, 88(%0)\n\t"
            "movq %%r12, 96(%0)\n\t"
            "movq %%r13, 104(%0)\n\t"
            "movq %%r14, 112(%0)\n\t"
            "movq %%r15, 120(%0)\n\t"
            :
            : "r" (regs)
            : "memory"
            );

    asm volatile ("mov %%cs, %0" : "=r" (regs->cs));
    asm volatile ("mov %%ds, %0" : "=r" (regs->ds));


    uint64_t rip_temp;
    asm volatile (
            "lea (%%rip), %0\n\t"
            : "=r" (rip_temp)
            );
    regs->rip = rip_temp;

    asm volatile ("pushfq\n\tpop %0" : "=r" (regs->rflags));
}
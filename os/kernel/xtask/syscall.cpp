#pragma once

#include "../CPU/cpu.cpp"
#include "scalls/scalls.cpp"
#include "xtask.cpp"

struct Syscall {
    __attribute__((naked))
    static void entry(void) {
        asm volatile(

                        "push %fs\n"
            "push %gs\n"
            "push %rax\n"
            "push %rcx\n"                          "push %rdx\n"
            "push %rbx\n"
            "push %rbp\n"
            "push %rsi\n"
            "push %rdi\n"
            "push %r8\n"
            "push %r9\n"
            "push %r10\n"
            "push %r11\n"                          "push %r12\n"
            "push %r13\n"
            "push %r14\n"
            "push %r15\n"







                        "pop %r15\n"
            "pop %r14\n"
            "pop %r13\n"
            "pop %r12\n"
            "pop %r11\n"
            "pop %r10\n"
            "pop %r9\n"
            "pop %r8\n"
            "pop %rdi\n"
            "pop %rsi\n"
            "pop %rbp\n"
            "pop %rbx\n"
            "pop %rdx\n"
            "pop %rcx\n"
            "pop %rax\n"
            "pop %gs\n"
            "pop %fs\n"

            "sysretq\n"
        );
    }

    static void init(void) {
        wrmsr(MSR_LSTAR, (uint64_t)entry);
        wrmsr(MSR_STAR, ((uint64_t)0x08 << 32) | ((uint64_t)0x10 << 48));
        wrmsr(MSR_FMASK, 0x200);

        uint64_t efer = rdmsr(MSR_EFER);
        efer |= EFER_SCE;
        wrmsr(MSR_EFER, efer);
    }
};

namespace xtask {
    void syscalls(){
        int cpu_id = smplow::get_core_id();

        wrmsr(MSR_KERNEL_GS_BASE, (uint64_t)&cores[cpu_id].gs);
        wrmsr(MSR_GS_BASE, 0x0);

        s0::put("GS setup for CPU ");
        s0::put(cpu_id);
        s0::put("\n");
    }
}

extern "C" uint64_t syscall_handler(xtask::context_t* regs) {
    uint64_t nr = regs->rax;

    if (nr >= sizeof scalls / sizeof &_hnSyscall::tester) {
        return -ENOSYS;
    }


    return scalls[nr](regs);
}

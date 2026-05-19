#pragma once

#include "xtask.cpp"
#include "stack.cpp"

namespace xtask {
    enum State {
        NEW,
        READY,
        RUNNING,
        BLOCKED,
        DEAD
    };

    struct Task {
        int tid;
        State s;
        context_t context;
        //interrupt_frame frame;
        uint8_t fpu_state[512] __attribute__((aligned(16)));
    };

    struct Process {
        uint64_t pid;
        uint64_t tcounter;
        Core* core;
        managed<Task*> tasks;
    };

    mutex procmut = {};

    uint64_t pcounter;

    void init() {
        cores = {};
        procmut = {};
        pcounter = 0;
    }

    Process* newprocess(const interrupt_frame &frame) {
        Core* core = cores::find_best();
        core->lock();

        Process* proc = new Process{
            .pid = pcounter++,
            .tcounter = 1,
            .core = core,
            .tasks = {},
        };
        proc->tasks.push_back(new Task {
            .tid = 0,
            .s = NEW,
            .context = {
                .rbp = 0,
                .fs = 0x10,
                .gs = 0x10,
                .rip = frame.ip,
                .cs = (uint16_t)frame.cs,
                .rflags = frame.flags,
                .rsp = frame.sp,
                .ss = (uint16_t)frame.ss,
            },
            //.frame = frame,
            .fpu_state = {}
        });
        core->tasks.push_back(proc->tasks[0]);
        core->unlock();

        return proc;
    }

    Process* runf(void (*func)()) {
        return newprocess({
            .ip = (uint64_t)func,
            .cs = 0x08,
            .flags = 0x202,
            .sp = (uint64_t)promise(64*1024)+64*1024,
            .ss = 0x10,
        });
    }

    extern "C" void sched(context_t* regs) {
        Core& core = cores[smplow::get_core_id()];
        core.lock();

        if (core.tasks.empty()) {
            core.unlock();
            return;
        }

        Task* prev = core.tasks.current();
        if (core.tasked) {
            prev->context = *regs;
            prev->s = READY;
        } else {
            core.tasked = true;
        }

        uint64_t cr0;
        asm volatile("mov %%cr0, %0" : "=r"(cr0));
        if (cr0 & 8) asm volatile("clts");
        asm volatile("fxsave %0" : "=m"(prev->fpu_state));

        Task* next = core.tasks.next();

        if (next->s == NEW) {
            asm volatile("fninit");
            memset(next->fpu_state, 0, 512);
            asm volatile("fxsave %0" : "=m"(next->fpu_state));
        }
        next->s = RUNNING;
        asm volatile("fxrstor %0" : : "m"(next->fpu_state));

        core.unlock();
        asm volatile(
            "mov %0, %%rsp\n"

            "pop %%gs\n"
            "pop %%fs\n"

            "pop %%r15\n"
            "pop %%r14\n"
            "pop %%r13\n"
            "pop %%r12\n"
            "pop %%r11\n"
            "pop %%r10\n"
            "pop %%r9\n"
            "pop %%r8\n"
            "pop %%rdi\n"
            "pop %%rsi\n"
            "pop %%rbp\n"
            "pop %%rbx\n"
            "pop %%rdx\n"
            "pop %%rcx\n"
            "pop %%rax\n"

            "mov $0x10, %%ax\n"
            "mov %%ax, %%ds\n"
            "mov %%ax, %%es\n"

            "iretq\n"
            :
            : "r"(&next->context)
            : "memory"
        );

        while(1) asm volatile("hlt");
    }
}

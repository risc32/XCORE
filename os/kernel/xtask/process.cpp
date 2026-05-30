#pragma once

#include "xtask.cpp"
#include "stack.cpp"
#include "usermem.cpp"

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
        void* krtop;
                uint8_t fpu_state[4096] __attribute__((aligned(16)));
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
        procmut = {};
        pcounter = 0;
    }

    extern "C" void sched(context_t* regs);

    void exit_task() {
        Core& core = cores[smplow::get_core_id()];
        core.lock();
        Task* t = core.tasks.current();
        t->s = DEAD;
                core.unlock();
        stop();
            }

    extern "C" void task_wrapper(void (*func)()) {
                                                func();
        exit_task();
    }

    Process* newprocess(const interrupt_frame &frame, Core* core = cores::find_best()) {
        core->lock();

        uint64_t stack_size = 16384;
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
                .rdi = frame.ip,
                .rbp = 0,
                .gs = 0x10,
                .fs = 0x10,
                .rip = (uint64_t)task_wrapper,
                .cs = (uint16_t)frame.cs,
                .rflags = frame.flags,
                .rsp = frame.sp,
                .ss = (uint16_t)frame.ss,
            },
            .krtop = (char*)promise(stack_size) + stack_size,
                        .fpu_state = {}
        });
        core->tasks.push_back(proc->tasks[0]);
        core->unlock();

        return proc;
    }

    Process* runfunc0(void (*func)(), Core* core) {
        return newprocess({
            .ip = (uint64_t)func,
            .cs = 0x08,
            .flags = 0x202,
            .sp = (uint64_t)promise(64*1024 + 128 + 16)+64*1024 - 8,
            .ss = 0x10,
        }, core);
    }

    Process* runfunc3(void (*func)(), Core* core = cores::find_best()) {
        return newprocess({
            .ip = (uint64_t)func,
            .cs = 0x1B,
            .flags = 0x202,
            .sp = (uint64_t)promise(64*1024 + 128 + 16)+64*1024 - 8,
            .ss = 0x23,
        }, core);
    }

    extern "C" void sched(context_t* regs) {
        Core& core = cores[smplow::get_core_id()];
        core.lock();


        if (core.tasks.empty()) {
            core.unlock();
            return;
        }

        Task* current = core.tasks.current();
        Task* next = core.tasks.next();
                if (core.tasked && current != next) {
            current->context = *regs;
            fxsave(current->fpu_state);
        }
        current->s = READY;
        next->s = RUNNING;
        fxrstor(next->fpu_state);

        core.tss.rsp0 = (uint64_t)next->krtop;
        core.tasked = true;
        core.unlock();
        asm volatile(
            "mov %0, %%rsp\n"

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
            "mov $0x10, %%ax\n"
            "mov %%ax, %%ds\n"
            "mov %%ax, %%es\n"
            "pop %%rax\n"

            "pop %%gs\n"
            "pop %%fs\n"


            "iretq\n"
            :
            : "r"(&next->context)
            : "memory"
        );


        while(1) asm volatile("hlt");
    }
}

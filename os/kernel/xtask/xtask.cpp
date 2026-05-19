#pragma once

#include "syscall.cpp"
#include "../crypto/crypto.cpp"
#include "../time/time.cpp"
#include "const.cpp"
#include "low.cpp"
#include "../async/async.cpp"
#include "../autotest/autotest.cpp"

namespace xtask {
    struct Task;
    struct Process;

    struct Core {
        uuid_t uuid;
        int id;
        Ringed<Task *> tasks;
        bool tasked;
        volatile uint32_t* lapic_base;
        mutex mt;

        void lock() {
            mt.lock();
        }
        void unlock() {
            mt.unlock();
        }
    };

    managed<Core> cores;

    static inline volatile uint32_t* get_lapic() {
        return (volatile uint32_t*)0xFEE00000;
    }

    struct interrupt_frame {
        uint64_t ip;
        uint64_t cs;
        uint64_t flags;
        uint64_t sp;
        uint64_t ss;
    };

    __attribute__((naked)) void tick() {
        asm volatile(
            //"cli\n"

            "push %rax\n"
            "push %rcx\n"
            "push %rdx\n"
            "push %rbx\n"
            "push %rbp\n"
            "push %rsi\n"
            "push %rdi\n"
            "push %r8\n"
            "push %r9\n"
            "push %r10\n"
            "push %r11\n"
            "push %r12\n"
            "push %r13\n"
            "push %r14\n"
            "push %r15\n"

            "push %fs\n"
            "push %gs\n"

            "mov $0xFEE000B0, %rax\n"
            "movl $0, (%rax)\n"

            "mov %rsp, %rdi\n"
            "call sched\n"

            "pop %gs\n"
            "pop %fs\n"

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

            "mov $0x10, %ax\n"
            "mov %ax, %ds\n"
            "mov %ax, %es\n"
            "mov %ax, %fs\n"
            "mov %ax, %gs\n"

            "iretq\n"
        );
    }
    void sf_entry() {
        //set_idt_gate64(32, (uint64_t)tick, 0x08, 0x8E);
        //sti();
    }

    struct mut {
        static mutex m;
    };
    mutex mut::m{};

    struct alignas(16) context_t {
        uint64_t r15, r14, r13, r12, r11, r10, r9, r8;
        uint64_t rdi, rsi, rbp, rbx, rdx, rcx, rax;
        uint64_t fs, gs;
        uint64_t rip, cs, rflags, rsp, ss;
    } __attribute__((packed));

    void init_lapic_per_cpu() {
        volatile uint32_t* lapic = get_lapic();

        uint32_t version = lapic[0x30 / 4];
        if (version == 0xFFFFFFFF) {
            s0::put("LAPIC not available on core ");
            s0::put(smplow::get_core_id());
            s0::put("\n");
            return;
        }

        lapic[LAPIC_SPURIOUS / 4] = 0x1FF;

        lapic[LAPIC_LVT_LINT0 / 4] = 0x10000;
        lapic[LAPIC_LVT_LINT1 / 4] = 0x10000;
        lapic[LAPIC_LVT_ERROR / 4] = 0x10000;

        lapic[LAPIC_TIMER_DIV_CONF / 4] = 0x03;

        lapic[LAPIC_LVT_TIMER / 4] = 0x20020;

        lapic[0x80 / 4] = 0;

        lapic[LAPIC_TIMER_INIT_COUNT / 4] = 0x1000000;

        asm volatile("mfence");

        s0::put("LAPIC initialized on core ");
        s0::put(smplow::get_core_id());
        s0::put(" (version: ");
        s0::put(version);
        s0::put(")\n");
    }

    void disable_pic() {
        outb(0x21, 0xFF);
        outb(0xA1, 0xFF);
        for (int i = 0; i < 100; i++) {
            asm volatile("pause");
        }
    }

    void enable_apic_mode() {
        uint64_t apic_base = rdmsr(0x1B);
        apic_base |= (1 << 11);
        apic_base &= ~(1 << 10);
        apic_base = (apic_base & ~0xFFF) | 0xFEE00000;
        wrmsr(0x1B, apic_base);
    }

    void init_per_cpu() {
        init_lapic_per_cpu();
        sf_entry();
    }

    void init_bsp() {

        s0::put("BSP initializing...\n");
        Core c = {};
        c.tasks = {};
        c.lapic_base = get_lapic();
        c.uuid = uuid4();
        c.id = 0;
        cores.push_back(c);
        disable_pic();
        enable_apic_mode();
        for (int i = 0; i < 1000; i++) {
            asm volatile("pause");
        }
        set_idt_gate64(32, (uint64_t)tick, 0x08, 0x8E);
        s0::put("BSP initialized\n");
    }

    void core_init() {
        if (smplow::get_core_id() == 0) {

            init_bsp();
        }
        init_per_cpu();
    }

    struct alignas(16) stack {
        static char stacks[17][1024*16];

    };
    char stack::stacks[17][1024*16]{};

    void entry(int core_id) {
        Core c = {};
        c.tasks = {};
        c.lapic_base = get_lapic();
        c.uuid = uuid4();
        c.id = core_id;
        cores.push_back(c);
    }

    void setup() {
        asm volatile("mov %0, %%rsp" : : "r"((uint64_t)stack::stacks[(smplow::lapic[0x20 / 4] >> 24)+1]));

        cli();
        mut::m.lock();

        load_idt64();
        s0::put("Core ");
        s0::put(smplow::get_core_id());
        s0::put(" starting...\n");

        asm volatile(
            "fninit\n"
            "movq %%cr0, %%rax\n"
            "andq $0xFFFFFFFFFFFFFFFB, %%rax\n"
            "orq $0x22, %%rax\n"
            "movq %%rax, %%cr0\n"
            "movq %%cr4, %%rax\n"
            "orq $0x600, %%rax\n"
            "movq %%rax, %%cr4\n"
            "fwait\n"
            : : : "rax", "memory"
        );

        init_fpu_extensions();

        core_init();

        //c.lapic_base = get_lapic();
        //c.tasks = {};
        //c.uuid = uuid4();
        //cores.push_back(c);
        //
        s0::put("Core ");
        s0::put(smplow::get_core_id());
        s0::put(" ready, waiting for interrupts...\n");
        mut::m.unlock();
        sti();
        entry(smplow::get_core_id());
        stop();
    }
}

#include "cores.cpp"
#include "process.cpp"

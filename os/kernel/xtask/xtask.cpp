#pragma once

#include "../crypto/crypto.cpp"
#include "../time/time.cpp"
#include "const.cpp"
#include "low.cpp"
#include "../async/async.cpp"
#include "../autotest/autotest.cpp"

namespace xtask {
    struct Task;
    struct Process;

    struct gstable {
        void* kstack_back = malloc(16384);
        void* kstack = (char*)kstack_back + 16384;

        void* ustack = nullptr;

        ~gstable() {
            free(kstack_back);
        }
    };

    struct Core {
        uuid_t uuid;
        int id;
        Ringed<Task *> tasks;
        bool tasked;
        volatile uint32_t* lapic_base;
        mutex mt;
        tss64 tss;
        bool present = true;
                gstable gs;

        void lock() {
            mt.lock();
        }
        void unlock() {
            mt.unlock();
        }
    };

    Core cores[16];
}
namespace cores {
    xtask::Core* find_best();
}
[[noreturn]] void update();
namespace xtask {
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
            "cli\n"

            "push %fs\n"
            "push %gs\n"

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

            "mov $0xFEE000B0, %rax\n"
            "movl $0, (%rax)\n"

            "mov %rsp, %rdi\n"
            "call sched\n"

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
            "mov $0x10, %ax\n"
            "mov %ax, %ds\n"
            "mov %ax, %es\n"
            "pop %rax\n"

            "pop %gs\n"
            "pop %fs\n"


            "iretq\n"
        );
    }
    void sf_entry() {
        uint64_t cr4;
        __asm__ volatile ("mov %%cr4, %0" : "=r"(cr4));
        cr4 |= (1 << 18);
        __asm__ volatile ("mov %0, %%cr4" : : "r"(cr4));
    }

    struct mut {
        static mutex m;
    };
    mutex mut::m{};

    struct alignas(16) context_t {
        uint64_t r15, r14, r13, r12, r11, r10, r9, r8;
        uint64_t rdi, rsi, rbp, rbx, rdx, rcx, rax;
        uint64_t gs, fs;
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

    void init_tss() {
        int core_id = smplow::get_core_id();
        static uint8_t boot_stacks[16][4096];
        uint64_t stack_top = (uint64_t)boot_stacks[core_id] + 4096;
        gdt_init_tss();
        cores[core_id].tss.rsp0 = stack_top;
        cores[core_id].tss.iomap_base = sizeof(tss64);

        gdt_init();
                                            }

    void syscalls();

    void init_per_cpu() {
        init_tss();

        init_lapic_per_cpu();
        sf_entry();
        syscalls();
    }
    Process* runfunc0(void (*func)(), Core* core = cores::find_best());

    void init_bsp() {
        s0::put("BSP initializing...\n");
        Core c = {};
        c.tasks = {};
        c.lapic_base = get_lapic();
        c.uuid = uuid4();
        c.id = 0;
        cores[0] = c;

        disable_pic();
        enable_apic_mode();
        for (int i = 0; i < 1000; i++) {
            asm volatile("pause");
        }
        set_idt_gate64(32, (uint64_t)tick, 0x08, 0xEE);
        load_gdt((uint64_t)&gdt_ptr);
        gdt_init();
        init_tss();
        runfunc0(update, cores);

        s0::put("BSP initialized\n");
    }

    void core_init() {
        if (smplow::get_core_id() == 0) {
            init_bsp();
        } else {
            load_gdt((uint64_t)&gdt_ptr);
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
        cores[core_id] = c;

        runfunc0(update, cores+core_id);
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

                                        //
        s0::put("Core ");
        s0::put(smplow::get_core_id());
        s0::put(" ready, waiting for interrupts...\n");
        mut::m.unlock();
        entry(smplow::get_core_id());
        sti();

        stop();
    }
}

#include "cores.cpp"
#include "process.cpp"
#include "syscall.cpp"

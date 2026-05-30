#pragma once

#include "../../graphics/text/console.hpp"
#include "../../types/scalar.cpp"
#include "../registers.cpp"
#include "../../stream/cstatic.cpp"
#include "../../debug/debug.cpp"

void *malloc(size_t size);
extern "C" void* __cxa_allocate_exception(size_t size) {
    return malloc(size);
}

void free(void *ptr);
extern "C" void __cxa_free_exception(void* obj) {
    free(obj);
}

typedef void (*isr_t)(registers_t *);
void load_idt64();
void init_idt64();

#include "../stacktrace.cpp"

struct exception {
    const static char *messages[];
    static isr_t interrupt_handlers[256];

    static void init() {
        s0::put("void exception::init() KERNEL 0x20000 .text\n");

        init_idt64();
        load_idt64();
        asm volatile ("cli");
    }
};

#define TRACE __PRETTY_FUNCTION__
#define panic(x) _panic(TRACE, x)

extern "C" void debug_trap() {
    asm volatile("int3");
}

struct PanicContext {
    bool halt = true;
    int errcode = 0;
    const char* func = "";
    const char* message = "";
    bool has_err = false;


    static void nullfunc(int, const char *, const char*){}

};
void _panic(const char* func, const char *message);

#include "../../managed/managed.cpp"

struct Exceptions {
    void init() {
        catchers = {};
        catchers.push({});
    }

    Stack<PanicContext> catchers;

    void setcatch(PanicContext ctx) {
        catchers.push(ctx);
    }

    void delcatch() {
        catchers.pop();
    }

    bool needhalt() {
        return catchers.peek().halt;
    }

    void func(int error_code, const char* func, const char *message) {
        catchers.peek().errcode = error_code;
        catchers.peek().func = func;
        catchers.peek().message = message;
        catchers.peek().has_err = true;
    }
} _globctx;
mutex _globcmut;

void setcatch(PanicContext ctx) {
    lock_guard {_globcmut};
    _globctx.setcatch(ctx);
}

void delcatch() {
    lock_guard {_globcmut};
    _globctx.delcatch();
}

bool needhalt() {
    return _globctx.needhalt();
}

void func(int error_code, const char* func, const char *message) {
    _globctx.func(error_code, func, message);
}

#include "../../xtask/low.cpp"

void _panic(const char* func, const char *message) {
    s0::put("void _panic()\n\r");
    s0::put(func);
    s0::put(": ");
    s0::put(message);
    s0::put("\n\r");

    _globctx.func(0, func, message);

    if (!_globctx.needhalt()) {
        return;
    }

    Console& console = _kcons::console;
    console.set_color(RED, BLACK);
    console.writeLine("\n\n", true);

    console.writeLine("", true);
    console.reset_color();

#ifdef stage2
    console.write("MIDDLE PANIC\nPress space for more information\nin ", true);
#else
    console.write("KERNEL PANIC\nPress space for more information\nin ", true);

#endif
    console.writeLine(func, true);
    console.write("\nMessage: ", true);
    console.write(message, true);
    console.writeLine("", true);
    console.write("PANIC ON CPU#");
    console.write((int)smplow::get_core_id());
    console.writeLine("");
    while (' ' != console.readChar()) {}
    console.clear();
#ifdef stage2
    console.write("MIDDLE PANIC\n", true);
#else
    console.write("KERNEL PANIC\n", true);
#endif
    console.writeLine(message, true);
    console.write("in ", true);
    console.writeLine(func, true);

#ifndef stage2
    debug_print_stacktrace(console);

#endif

    console.writeLine("System halted", true);
    console.write("PANIC ON CPU#");
    console.write((int)smplow::get_core_id());
    console.writeLine("");

    while (true) { asm volatile ("hlt"); }
}

[[noreturn]] void stop() {
        while (true) { asm volatile ("hlt"); }
}

extern "C" {
void isr0();
void isr1();
void isr2();
void isr31();
}

const char *exception::messages[] = {
        "Division By Zero",
        "Debug",
        "Non Maskable Interrupt",
        "Breakpoint",
        "Into Detected Overflow",
        "Out of Bounds",
        "Invalid Opcode",
        "No Coprocessor",
        "Double Fault",
        "Coprocessor Segment Overrun",
        "Bad TSS",
        "Segment Not Present",
        "Stack Fault",
        "General Protection Fault",
        "Page Fault",
        "Unknown Interrupt",
        "Coprocessor Fault",
        "Alignment Check",
        "Machine Check",
        "Reserved",
        "Reserved",
        "Reserved",
        "Reserved",
        "Reserved",
        "Reserved",
        "Reserved",
        "Reserved",
        "Reserved",
        "Reserved",
        "Reserved",
        "Reserved",
        "Reserved"
};

isr_t exception::interrupt_handlers[256] = {};

#include "fault.cpp"
#include "idt.cpp"

template<auto (*Handler)()>
struct AsmHandler {
    static auto stub() {
        asm volatile(
            "pushq %%rax\n\t"
            "pushq %%rbx\n\t"
            "pushq %%rcx\n\t"
            "pushq %%rdx\n\t"
            "pushq %%rsi\n\t"
            "pushq %%rdi\n\t"
            "pushq %%rbp\n\t"
            "pushq %%r8\n\t"
            "pushq %%r9\n\t"
            "pushq %%r10\n\t"
            "pushq %%r11\n\t"
            "pushq %%r12\n\t"
            "pushq %%r13\n\t"
            "pushq %%r14\n\t"
            "pushq %%r15\n\t"

            "cld\n\t"
            "call %P0\n\t"

            "popq %%r15\n\t"
            "popq %%r14\n\t"
            "popq %%r13\n\t"
            "popq %%r12\n\t"
            "popq %%r11\n\t"
            "popq %%r10\n\t"
            "popq %%r9\n\t"
            "popq %%r8\n\t"
            "popq %%rbp\n\t"
            "popq %%rdi\n\t"
            "popq %%rsi\n\t"
            "popq %%rdx\n\t"
            "popq %%rcx\n\t"
            "popq %%rbx\n\t"
            "popq %%rax\n\t"

            "iretq\n\t"
            : : "p"(Handler) : "memory"
        );
    }
};

template<void (*Handler)()>
static auto asm_handler = (void*)AsmHandler<Handler>::stub;

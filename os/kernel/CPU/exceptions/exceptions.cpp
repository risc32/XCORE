#pragma once

#include "../../console.cpp"
#include "../../types/scalar.cpp"
#include "../registers.cpp"
#include "../../stream/cstatic.cpp"
#include "../../debug/debug.cpp"

typedef void (*isr_t)(registers_t *);
void load_idt64();
void init_idt64();
struct exception {
    const static char *messages[];
    static isr_t interrupt_handlers[256];

    static void init() {  ///for stacktrace
        s0::put("void exception::init() KERNEL 0x20000 .text\n");

        init_idt64();
        load_idt64();
        asm volatile ("cli");
    }
};

#define TRACE __PRETTY_FUNCTION__
#define panic(x) _panic(TRACE, x)

[[noreturn]] void _panic(const char* func, const char *message) {
    Console& console = _kcons::console;
    console.set_color(RED, BLACK);
    console.writeLine("\n\n");
    int cy = console.cursor_y;
    for (int i = 0; i < 80; ++i) {
        console.write("=");
    }
    console.place(" FAULT ", 36, cy);
    console.writeLine("");
    console.reset_color();

#ifdef stage2
    console.write("MIDDLE PANIC\nPress space for more information\nin ");
#else
    console.write("KERNEL PANIC\nPress space for more information\nin ");
#endif
    console.writeLine(func);
    console.write("\nMessage: ");
    console.write(message);
    console.writeLine("");
    while (' ' != console.readChar()) {}
    console.clear();
#ifdef stage2
    console.write("MIDDLE PANIC\n");
#else
    console.write("KERNEL PANIC\n");
#endif
    console.writeLine(message);
    console.write("in ");
    console.writeLine(func);

#ifndef stage2
    debug_print_stacktrace(console);
    FunctionTracer::dump_trace(console);
#endif

    console.writeLine("System halted");
    //if(interrupt) console.place("PROCESSOR INTERRUPT", 0, 24 );

    while (true) { asm volatile ("hlt"); }
}

void stop() {  ///for stacktrace

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

#pragma once

#include "../graphics/text/console.hpp"
#include "cpu.cpp"

struct StackFrame {
    StackFrame *next;
    uint32_t return_addr;
};

extern "C" void debug_print_stacktrace(Console &console, bool skip = true, uint32_t max_frames = 10) {
    console.writeLine("\nStack trace:", true);

    StackFrame *frame;
    asm volatile ("mov %%rbp, %0" : "=r"(frame));

    bool interrupt = false;

    int skipcount = 4;

    for (uint32_t i = 0; frame && i < max_frames; i++) {
        if (skip && skipcount) {
            skipcount--;
            i--;
            frame = frame->next;
            continue;
        }

        console.write("  #", true);
        console.write((int)i);
        console.write(": [EBP=", true);
        console.writeHex((uint32_t) frame);
        console.write("] RET=", true);
        console.writeHex(frame->return_addr);
        if (interrupt) {
            console.write(" <-- INTERRUPT", true);
            interrupt = false;
        }
        if (frame->return_addr == 0x6) {
            interrupt = true;
        }
        console.writeLine("", true);

        if (!frame->next || (uint32_t) frame->next < 0x10000 || (uint32_t) frame->next > 0x9FFFF)
            break;

        frame = frame->next;
    }
}

extern "C" void debug_dump_full_context(registers_t *regs, Console &console) {

    console.writeLine("\n=== FULL EXCEPTION CONTEXT ===", true);

    console.write("EAX: ", true);
    console.writeHex(regs->rax);
    console.write(" EBX: ", true);
    console.writeHex(regs->rbx);
    console.write(" ECX: ", true);
    console.writeHex(regs->rcx);
    console.write(" EDX: ", true);
    console.writeHex(regs->rdx);
    console.writeLine("", true);

    console.write("ESP: ", true);
    console.writeHex(regs->rsp);
    console.write(" EBP: ", true);
    console.writeHex(regs->rbp);
    console.write(" EIP: ", true);
    console.writeHex(regs->rip);
    console.writeLine("", true);

    console.writeLine("\nStack around EBP:", true);
    uint32_t *stack_ptr = (uint32_t *) (regs->rbp & 0xFFFFFFF0);

    for (int i = 0; i < 8; i++) {
        console.write("  [", true);
        console.writeHex((uint32_t) stack_ptr);
        console.write("] = ", true);
        console.writeHex(*stack_ptr);

        if (stack_ptr == (uint32_t *) regs->rbp)
            console.write(" <-- EBP", true);
        else if (*stack_ptr == regs->rip)
            console.write(" <-- EIP", true);

        console.writeLine("", true);
        stack_ptr++;
    }

    debug_print_stacktrace(console);
}
#include "../console.cpp"
#include "cpu.cpp"

struct StackFrame {
    StackFrame *next;
    uint32_t return_addr;
};

extern "C" void debug_print_stacktrace(Console &console, bool skip = false, uint32_t max_frames = 10) {
    console.writeLine("\nStack trace:");

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

        console.write("  #");
        console.write(i);
        console.write(": [EBP=");
        console.writeHex((uint32_t) frame);
        console.write("] RET=");
        console.writeHex(frame->return_addr);
        if (interrupt) {
            console.write(" <-- INTERRUPT");
            interrupt = false;
        }
        if (frame->return_addr == 0x6) {
            interrupt = true;
        }
        console.writeLine("");

        if (!frame->next || (uint32_t) frame->next < 0x10000 || (uint32_t) frame->next > 0x9FFFF)
            break;

        frame = frame->next;
    }
}

extern "C" void debug_dump_full_context(registers_t *regs, Console &console) {

    console.writeLine("\n=== FULL EXCEPTION CONTEXT ===");

    console.write("EAX: ");
    console.writeHex(regs->rax);
    console.write(" EBX: ");
    console.writeHex(regs->rbx);
    console.write(" ECX: ");
    console.writeHex(regs->rcx);
    console.write(" EDX: ");
    console.writeHex(regs->rdx);
    console.writeLine("");

    console.write("ESP: ");
    console.writeHex(regs->rsp);
    console.write(" EBP: ");
    console.writeHex(regs->rbp);
    console.write(" EIP: ");
    console.writeHex(regs->rip);
    console.writeLine("");

    console.writeLine("\nStack around EBP:");
    uint32_t *stack_ptr = (uint32_t *) (regs->rbp & 0xFFFFFFF0);

    for (int i = 0; i < 8; i++) {
        console.write("  [");
        console.writeHex((uint32_t) stack_ptr);
        console.write("] = ");
        console.writeHex(*stack_ptr);

        if (stack_ptr == (uint32_t *) regs->rbp)
            console.write(" <-- EBP");
        else if (*stack_ptr == regs->rip)
            console.write(" <-- EIP");

        console.writeLine("");
        stack_ptr++;
    }

    debug_print_stacktrace(console);
}
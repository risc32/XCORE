#pragma once

#include "../../console.cpp"
#include "../cpu.cpp"
#include "../../utils/basicmath.cpp"
#include "../../memory/basic/memchr.cpp"

void find_ud2_in_memory(Console console) {
    uint8_t *kernel_start = (uint8_t *) 0x10000;
    uint32_t kernel_size = 0x10000;

    for (uint32_t i = 0; i < kernel_size - 1; i++) {
        if (kernel_start[i] == 0x0F && kernel_start[i + 1] == 0x0B) {
            console.write("Found UD2 at: ");
            console.writeHex(0x10000 + i);
            console.writeLine("");
        }
    }
}


void fault_handler(registers_t *regs) {

    uint32_t original_eip;


    asm volatile (
            "mov 8(%%esp), %0\n\t"
            : "=r" (original_eip)
            :
            : "memory"
            );

    regs->eip = original_eip;

    if (regs->int_no > 31) {
        panic("UNKNOWN");
    }
    panic(exception::messages[regs->int_no]);

}

static void page_fault_handler(registers_t *regs) {
    uint32_t faulting_address;
    asm volatile("mov %%cr2, %0" : "=r" (faulting_address));

    Console console;
    console.write("Page Fault at ");
    console.writeHex(faulting_address);
    console.writeLine("");

    while (1) asm volatile("hlt");
}

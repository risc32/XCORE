#pragma once

#include "../types/types.cpp"
//#include "../stream/stream.cpp"

void enable_pae() {
    uint32_t cr4;
    asm volatile("mov %%cr4, %0" : "=r"(cr4));
    cr4 |= (1 << 5);
    asm volatile("mov %0, %%cr4" : : "r"(cr4));
}

void enable_long_mode() {

    uint32_t efer;
    asm volatile(
            "mov $0xC0000080, %%ecx\n"
            "rdmsr\n"
            "mov %%eax, %0"
            : "=r"(efer)
            :
            : "%ecx", "%edx"
            );

    efer |= (1 << 8);

    asm volatile(
            "mov %0, %%eax\n"
            "mov $0xC0000080, %%ecx\n"
            "wrmsr"
            :
            : "r"(efer)
            : "%eax", "%ecx", "%edx"
            );
}

void enable_paging_64bit() {
    uint32_t cr0;
    asm volatile("mov %%cr0, %0" : "=r"(cr0));
    cr0 |= (1 << 31);
    asm volatile("mov %0, %%cr0" : : "r"(cr0));
}
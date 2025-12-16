#pragma once

#include "../types/types.cpp"

void enable_sse() {

    uint64_t cr0;
    asm volatile("mov %%cr0, %0" : "=r"(cr0));
    cr0 &= ~(1 << 2);
    cr0 |= (1 << 1);
    asm volatile("mov %0, %%cr0" :: "r"(cr0));


    uint64_t cr4;
    asm volatile("mov %%cr4, %0" : "=r"(cr4));
    cr4 |= (3 << 9);
    asm volatile("mov %0, %%cr4" :: "r"(cr4));
}

uint64_t read_xcr(uint32_t xcr) {
    uint32_t eax, edx;
    asm volatile("xgetbv" : "=a"(eax), "=d"(edx) : "c"(xcr));
    return ((uint64_t)edx << 32) | eax;
}

void write_xcr(uint32_t xcr, uint64_t value) {
    uint32_t eax = (uint32_t)value;
    uint32_t edx = (uint32_t)(value >> 32);
    asm volatile("xsetbv" :: "c"(xcr), "a"(eax), "d"(edx));
}

void enable_avx() {
    uint32_t eax, ebx, ecx, edx;


    asm volatile("cpuid"
            : "=a"(eax), "=b"(ebx), "=c"(ecx), "=d"(edx)
            : "a"(1), "c"(0));

    if (!(ecx & (1 << 27))) {

        return;
    }


    if (!(ecx & (1 << 28))) {

        return;
    }


    uint64_t cr4;
    asm volatile("mov %%cr4, %0" : "=r"(cr4));
    cr4 |= (1 << 18);
    asm volatile("mov %0, %%cr4" :: "r"(cr4) : "memory");


    uint64_t xcr0 = read_xcr(0);


    xcr0 |= (1 << 0);
    xcr0 |= (1 << 1);
    xcr0 |= (1 << 2);


    asm volatile("cpuid"
            : "=a"(eax), "=b"(ebx), "=c"(ecx), "=d"(edx)
            : "a"(7), "c"(0));

    if (ebx & (1 << 16)) {
        xcr0 |= (1 << 5);
        xcr0 |= (1 << 6);
        xcr0 |= (1 << 7);
    }


    write_xcr(0, xcr0);
}
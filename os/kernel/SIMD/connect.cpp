#pragma once

#include "../types/types.cpp"

bool ssesupp() {
    uint32_t eax, ebx, ecx, edx;


    asm volatile("cpuid"
            : "=a"(eax), "=b"(ebx), "=c"(ecx), "=d"(edx)
            : "a"(1));

    return (edx >> 25) & 1;
}

bool avxsupp() {
    uint32_t eax, ebx, ecx, edx;

    asm volatile("cpuid"
            : "=a"(eax), "=b"(ebx), "=c"(ecx), "=d"(edx)
            : "a"(1));


    return ((ecx >> 27) & 1) && ((ecx >> 28) & 1) && ((ecx >> 20) & 1);
}

bool avx512supp() {
    uint32_t eax, ebx, ecx, edx;

    asm volatile("cpuid"
            : "=a"(eax), "=b"(ebx), "=c"(ecx), "=d"(edx)
            : "a"(7), "c"(0));

    return (ebx >> 16) & 1;
}

#include "copy.cpp"
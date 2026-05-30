#pragma once

#ifndef stage2
#include "trace.cpp"
#include "registers.cpp"
#include "stacktrace.cpp"
#endif
#include "ports.cpp"
#include "../async/async.cpp"
#include "exceptions/exceptions.cpp"
#include "msr.cpp"

#ifndef stage2

#define OPT3 __attribute__((optimize(3)))
#define OPT2 __attribute__((optimize(2)))
#define OPT1 __attribute__((optimize(1)))
#define OPT0 __attribute__((optimize(0)))

static inline void cli() {
    asm volatile("cli" ::: "memory");
}

static inline void sti() {
    asm volatile("sti" ::: "memory");
}

static inline void xsave(void* buffer) {
    uint64_t mask_edx = 0xFFFFFFFF;
    uint64_t mask_eax = 0xFFFFFFFF;

    __asm__ volatile (
        "xsave (%%rdi)\n\t"
        :
        : "D" (buffer), "a" (mask_eax), "d" (mask_edx)
        : "memory"
    );
}

static inline void xrstor(void* buffer) {
    uint64_t mask_edx = 0xFFFFFFFF;
    uint64_t mask_eax = 0xFFFFFFFF;

    __asm__ volatile (
        "xrstor (%%rdi)\n\t"
        :
        : "D" (buffer), "a" (mask_eax), "d" (mask_edx)
        : "memory"
    );
}

static inline void fxsave(void* buffer) {
    __asm__ volatile ("fxsave (%%rdi)" : : "D"(buffer) : "memory");
}

static inline void fxrstor(void* buffer) {
    __asm__ volatile ("fxrstor (%%rdi)" : : "D"(buffer) : "memory");
}

#include "rtti.cpp"
#include "gdt.cpp"
#endif
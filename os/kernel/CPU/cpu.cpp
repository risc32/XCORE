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

#include "rtti.cpp"
#endif
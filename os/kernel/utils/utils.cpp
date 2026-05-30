#pragma once

#include "itoa.cpp"
#include "../types/types.cpp"

#include "atoi.cpp"
#ifndef stage2

#include "strings.cpp"
#include "basicmath.cpp"
#include "bitwise.cpp"
#include "division64.cpp"
#endif

#define INTEL(...) asm volatile ("\n.intel_syntax noprefix");asm volatile (__VA_ARGS__ ); asm volatile(".att_syntax prefix")

#define IRR(cmd, src, dest) \
    INTEL(#cmd " " #src ", " #dest)

#define IVR(cmd, var, reg) \
    INTEL(#cmd " %0, " #reg : "=r"(var));

#define IRV(cmd, reg, var) \
    INTEL(#cmd " " #reg ", %0" : "=r"(var));

#define IRC(cmd, reg, const_val) \
    INTEL(#cmd " " #reg ", " #const_val);

#define RESERVE_MEMORY(name, addr, size) \
    volatile __attribute__((section(".reserved." #addr))) \
    uint8_t name##_placeholder[size] __attribute__((unused))

RESERVE_MEMORY(vga, 0xB8000, 80*25*2);

template<typename T>
constexpr T align_up(T value, T alignment) {
    return (value + alignment - 1) & ~(alignment - 1);
}

constexpr uint64_t align_up(uint64_t value, uint64_t alignment) {
    return (value + alignment - 1) & ~(alignment - 1);
}

template <typename A, typename B>
struct pair {
    A first;
    B second;
};


#define GLOBAL(type, var) struct _g_##type{static type* var; static char buf[sizeof(type)]; static void init() {var = new (buf) type;} static type* get() {if (!var) {var = new (buf) type;} return var;}}; type* _g_##type::var = (type*)_g_##type::buf; char _g_##type::buf[sizeof(type)]{}; auto var = *_g_##type::get();
#define INIT(type, var) _g_##type::init();

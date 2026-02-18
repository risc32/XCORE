#pragma once

#include "../utils/utils.cpp"


#define MSR_EFER        0xC0000080
#define MSR_STAR        0xC0000081
#define MSR_LSTAR       0xC0000082
#define MSR_CSTAR       0xC0000083
#define MSR_FMASK      0xC0000084
#define MSR_FS_BASE     0xC0000100
#define MSR_GS_BASE     0xC0000101
#define MSR_KERNEL_GS_BASE 0xC0000102


#define EFER_SCE        (1 << 0)
#define EFER_LME        (1 << 8)
#define EFER_LMA        (1 << 10)
#define EFER_NXE        (1 << 11)


static inline uint64_t rdmsr(uint32_t msr) {
    uint32_t low, high;
    asm volatile("rdmsr" : "=a"(low), "=d"(high) : "c"(msr));
    return ((uint64_t)high << 32) | low;
}

static inline void wrmsr(uint32_t msr, uint64_t value) {
    uint32_t low = (uint32_t)value;
    uint32_t high = (uint32_t)(value >> 32);
    asm volatile("wrmsr" : : "c"(msr), "a"(low), "d"(high));
}


static inline uint16_t get_cs(void) {
    uint16_t seg;
    asm volatile("mov %%cs, %0" : "=rm"(seg));
    return seg;
}

static inline uint16_t get_ss(void) {
    uint16_t seg;
    asm volatile("mov %%ss, %0" : "=rm"(seg));
    return seg;
}

static inline uint16_t get_ds(void) {
    uint16_t seg;
    asm volatile("mov %%ds, %0" : "=rm"(seg));
    return seg;
}

static inline uint16_t get_es(void) {
    uint16_t seg;
    asm volatile("mov %%es, %0" : "=rm"(seg));
    return seg;
}

static inline uint16_t get_fs(void) {
    uint16_t seg;
    asm volatile("mov %%fs, %0" : "=rm"(seg));
    return seg;
}

static inline uint16_t get_gs(void) {
    uint16_t seg;
    asm volatile("mov %%gs, %0" : "=rm"(seg));
    return seg;
}

static inline void load_tr(uint16_t sel) {
    asm volatile("ltr %0" : : "r"(sel));
}
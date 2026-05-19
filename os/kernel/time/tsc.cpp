#pragma once

#include "../CPU/cpu.cpp"

static inline uint64_t rdtsc() {
    uint32_t lo, hi;
    __asm__ __volatile__ (
            "rdtsc"
            : "=a"(lo), "=d"(hi)
            );
    return ((uint64_t)hi << 32) | lo;
}

uint64_t get_tsc_freq(void) {

    __asm__ volatile ("cli");

    outb(0x43, 0x34);
    outb(0x40, 0x00);
    outb(0x40, 0x00);

    uint64_t start = rdtsc();

    for (int i = 0; i < 10000; i++) {
        while ((inb(0x40) & 0x80) == 0);
    }
    uint64_t end = rdtsc();
    //stop();

    //__asm__ volatile ("sti");

    return (end - start) * 100;
}

int tscfreq = get_tsc_freq();

void wait(uint32_t ms) {
    uint64_t freq_hz = tscfreq;
    uint64_t start = rdtsc();
    uint64_t needed = (ms * freq_hz) / 1000;

    while ((rdtsc() - start) < needed);
}

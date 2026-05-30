#pragma once

#include "../CPU/cpu.cpp"

#define PIT_CHANNEL0_DATA 0x40
#define PIT_COMMAND_REG   0x43

#define PIT_MODE_RATE_GEN  0x34
#define PIT_MODE_SQUARE    0x36

uint16_t pit_init(uint32_t hz) {
    if (hz == 0) return 0;
    uint16_t divisor = 1193182 / hz;
    outb(PIT_COMMAND_REG, PIT_MODE_SQUARE);
    outb(PIT_CHANNEL0_DATA, divisor & 0xFF);
    outb(PIT_CHANNEL0_DATA, (divisor >> 8) & 0xFF);

    return divisor;
}

void pit_init25() {
    pit_init(40);

    asm volatile("sti");
}

void pit_initms(int ms) {
    pit_init(1000 / ms);

    asm volatile("sti");
}

void io_delay_pit(uint32_t microseconds) {
    uint32_t count = (uint32_t)((uint64_t)microseconds * 1193182 / 1000000);

    uint8_t old_mode = inb(0x61);

    outb(0x43, 0xB0);
    outb(0x42, count & 0xFF);
    outb(0x42, count >> 8);

    while (!(inb(0x61) & 0x20)) {}

    outb(0x61, old_mode & ~0x01);
}
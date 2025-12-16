#pragma once

#include "../types/scalar.cpp"

static inline uint8_t inb(uint16_t port) {
    uint8_t result;
    asm volatile ("inb %w1, %b0"
            : "=a" (result)
            : "Nd" (port));
    return result;
}

static inline void outb(uint16_t port, uint8_t data) {
    asm volatile ("outb %b0, %w1"
            :
            : "a" (data), "Nd" (port));
}

static inline uint16_t inw(uint16_t port) {
    uint16_t result;
    asm volatile ("inw %w1, %w0"
            : "=a" (result)
            : "Nd" (port));
    return result;
}

static inline void outw(uint16_t port, uint16_t data) {
    asm volatile ("outw %w0, %w1"
            :
            : "a" (data), "Nd" (port));
}
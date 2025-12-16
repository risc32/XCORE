#pragma once

#include "../cpu/ports.cpp"
#include "../utils/utils.cpp"

namespace s0{
    void put(const char* c) {
        for (const char* p = c; *p; p++) {
            while ((inb(0x3F8 + 5) & 0x20) == 0);
            if (*p == '\n') outb(0x3F8, '\r');
            outb(0x3F8, *p);
        }
    }

    void put(int c) {
        char b[8] = {};
        itoa(c, b, 10);
        put(b);
    }

    void puthex(int c) {
        char b[8] = {};
        itoa(c, b, 16);
        put(b);
    }
}


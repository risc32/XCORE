#pragma once

#include "../cpu/ports.cpp"
#include "../utils/utils.cpp"

namespace s0{
    void put(const char* c) {
#ifndef stage2
        for (const char* p = c; *p; p++) {
            while ((inb(0x3F8 + 5) & 0x20) == 0);
            if (*p == '\n') outb(0x3F8, '\r');
            outb(0x3F8, *p);
        }
#endif
    }

    void put(int c) {
#ifndef stage2
        char b[8] = {};
        itoa(c, b, 10);
        put(b);
#endif
    }

    void puthex(int c) {
#ifndef stage2
        char b[8] = {};
        itoa(c, b, 16);
        put(b);
#endif
    }
}


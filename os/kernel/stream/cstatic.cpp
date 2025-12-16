#pragma once

#include "../console.cpp"

struct _kcons {
    static Console console;

    static void init() {  ///for stacktrace
        s0::put("void _kcons::init() KERNEL 0x20000 .text\n");

        console = Console();
    }
};

void waitChar(char c) {
    while (_kcons::console.readChar() != c);
}

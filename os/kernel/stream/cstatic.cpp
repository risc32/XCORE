#pragma once

#include "../graphics/text/console.hpp"
#include "../debug/debug.cpp"

struct _kcons {
    static Console console;

    static void init() {
        s0::put("void _kcons::init() KERNEL 0x20000 .text\n");

        console = Console();
    }
};

#define mcon _kcons::console

void waitChar(char c) {
    while (_kcons::console.readChar() != c);
}

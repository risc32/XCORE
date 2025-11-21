#pragma once

#include "../console.cpp"

struct _kcons {
    static Console console;

    static void init() {  ///for stacktrace

        console = Console();
    }
};

void waitChar(char c) {
    while (_kcons::console.readChar() != c);
}
Console _kcons::console = Console();
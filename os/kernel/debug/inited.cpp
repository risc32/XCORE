#pragma once

#include "debug.cpp"
#include "../stream/stream.cpp"

struct _serial0: ostream {
    void _flush() override {
        s0::put(string(buffer.c_str()).c_str());
    }

    static void init();
} serial0;

void _serial0::init() {
    serial0 = {};
}

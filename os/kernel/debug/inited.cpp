#pragma once

#include "debug.cpp"
#include "../stream/stream.cpp"

struct serial0 : ostream {
    void _flush() override {
        s0::put(string(buffer.c_str()).c_str());
    }
};

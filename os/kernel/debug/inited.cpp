#pragma once

#include "debug.cpp"
#include "../stream/stream.cpp"

struct _serial0: ostream {
    void _flush() override {
        s0::put(string(buffer.c_str()).c_str());
    }

    _serial0 () : ostream{0x6266} {}

    };
//
GLOBAL(_serial0, serial0);




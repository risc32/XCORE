#pragma once

#include "../types/scalar.cpp"
#include "../cpu/ports.cpp"
#include "../managed/managed.cpp"
#include "../stream/stream.cpp"
#include "../utils/inited.cpp"
#include "cmd.cpp"
#include "../debug/inited.cpp"

int cmdsr(const managed<string>& args) {
    KernelOut kout;


    CHECK(1)
    string com = args[0];
    if (com == "test") {
        serial0() << "testing serial0 out port" << endl;
    } else {
        kout << "UNKNOWN COMMAND: " << '"' << com << '"' << endl;
        return 1;
    }



    return 0;
}

int mmstress(argt args) {
    CHECK(1)
    int c = to_int(args[0]);
    void** ptrs = (void**)calloc(64, sizeof(void*));

    for (int j = 1; j < c; ++j) {
        for (int i = 1; i < 64+1; ++i) {
            ptrs[i-1] = allocate(i * 8);
        }
        for (int i = 0; i < 64; ++i) {
            free(ptrs[i]);
        }
    }

    free(ptrs);
    return 0;
}

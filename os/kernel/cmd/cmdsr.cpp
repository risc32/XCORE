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

    //kout << (args[0] == "readsec") << ":" << args[1].size() << endl;

    return 0;
}
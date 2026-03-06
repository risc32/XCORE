#define ydiskans
#define noallocdir

#include "init.cpp"

#ifndef stage2

bool mmstmin() {
    constexpr int count = 32;
    void* p[count]{};
    for (int i = 0; i < count; ++i) {
        p[i] = allocate(128);

    }
    for (int i = count - 1; i >= 0; --i) {
        if (!p[i]) {
            return false;
        }
        free(p[i]);
    }
    return true;
}

extern "C" [[noreturn]] void _start() {
    init();


    KernelOut kout = KernelOut();
    KernelIn kin = KernelIn();
    kout.clear();
    kout << reset;
    kout << WHITE << "XCore";
    kout << reset << " KERNEL v2.0 / Type 'help' for information" << endl;







































    mathtest();
    string com = "";
    while (true) {
        kout << "> ";
        kin.clear();
        com = KernelIn::readLine();
        cmd::execute(com);
    }
}

#endif
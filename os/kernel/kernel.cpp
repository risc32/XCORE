#define ydiskans
#define noallocdir

#include "init.cpp"

#ifndef stage2


void tester() {
    static mutex m;
        CPUFACE;

    void* g;
        Request f{};
        asm volatile("syscall" :: "a"(0));
                    while (1) {
        yield();
    }
    stop();
    wait(700);

}


extern "C" [[noreturn]] void _start() {
    cli();
    init();

    wait(2250);
    xtask::core_init();
    cores::startup();

            xtask::runfunc3(tester);

    sti();
        stop();
    Screen::clear();

    KernelOut kout = KernelOut();
    KernelIn kin = KernelIn();
    kout.clear();
    kout << reset;
    kout << WHITE << "XCore";
    kout << reset << " KERNEL v2.0 / Type 'help' for information" << endl;

                                    string com = "";
                cores::c = 0;
                                    kout << speedmessage::alloc << endl;
    kout << speedmessage::cpus << endl;

    Screen::frame();

    while (true) {
        kout << "> ";
        kin.clear();
        com = KernelIn::readLine();
        cmd::execute(com);
    }
}

#endif
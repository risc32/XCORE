extern "C" [[noreturn]] void _start();
extern "C" [[noreturn]] void stage();

extern "C" void kernel_entry() {
    stage();
}
//#define DEBUG
#define main
#include "console.cpp"
#include "cpu/cpu.cpp"
#include "disk/disk.cpp"

extern "C" [[noreturn]] void stage() {
    exception::init();
    _kcons::init();
    disk::init();
    disk::read(SECOND+128, 128, (char*)0x20000);
    _start();
}

#include "autotest/autotest.cpp"
#include "cmd/cmd.cpp"
#include "managed/managed.cpp"
#include "memory/memory.cpp"
#include "stream/stream.cpp"
#include "xcfs/xcfs.cpp"
#include "time/time.cpp"

extern "C" [[noreturn]] void  _start() {
    //FunctionTracerRAII __tracer_raii{"_function_name_", (int) __builtin_return_address(0)};
    outb(0x3D4, 0x0A);
    outb(0x3D5, 0x20);
    exception::init();
    _kcons::init();
    disk::init();
    disk::read(SECOND+128, 128, (char*)0x20000);
    memory::init();
    autotest();

    cmd::init();
    KernelOut kout = KernelOut();
    KernelIn kin = KernelIn();
    filesystem::init();

    filesystem::mount();

    kout.clear();



    string com1 = "";
    //console.write(a.data());
    //asm ("ud2");




    //kout << RED;
    //kout << "  ";
    kout << WHITE << "XCore";
    kout << reset << " KERNEL v1.2 / Type 'help' for information " << endl << endl;


    string com = "";
    while (true) {
        kout << "> ";


        kin.clear();

        com = KernelIn::readLine();

        //kout << get_rtc_time().getstring() << endl;

        cmd::execute(com);
    }
}

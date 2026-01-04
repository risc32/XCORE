extern "C" [[noreturn]] void _start();
extern "C" [[noreturn]] void endloader();
extern "C" void middle();

extern "C" void kernel_entry() {
#ifdef stage2
    middle();
#else
    endloader();
#endif
}

namespace s0{
    void put(const char* c);

    void put(int c);

    void puthex(int c);
}

#include "console.cpp"
#include "CPU/cpu.cpp"

//#include "cpu/cpu.cpp"
//#define DEBUG
#define main
#include "disk/atadriver.cpp"
#include "xcfs/storage.cpp"
#include "memory/memory.cpp"
#include "debug/debug.cpp"



//#define stage2
//#include "load64/bit64.cpp"
#ifdef stage2
extern "C" void middle() {

volatile unsigned short* vga = (volatile unsigned short*)0xB8000;
    exception::init();
    _kcons::init();

    s0::put("MIDDLE STAGE 0x15000 .text\n");
    Console& console = _kcons::console;
    console.clear();
    ATADriver driver = {};

    outb(0x3D4, 0x0A);
    outb(0x3D5, 0x20);
    super_block sb = {};
    driver.read(1, 1, sb.data);

    uint64_t kernel_address = 0;

    int sectors = 1024;

    if (sb.kerneladdr == 0) {
        console.writeLine("Kernel address is null - loading from sector 39\n");
        sb.kerneladdr = 39;
    } else {
        console.writeLine("Loading kernel from filesystem\n");
    }



    for (int i = 0; i < sectors / 256; i++) {
        console.write(".");
        driver.read(sb.kerneladdr + i * 256, 256, (char*)KERNELADDR + i * 512 * 256);
    }

    console.writeLine("Kernel loaded at "STRKDRR"\n");

    //use64();
    //INTEL("mov rsp, 0x7000");
    INTEL("jmp "STRKDRR);
    panic("Returned from kernel");
}
#else

#include "stream/cstatic.cpp"
#include "SIMD/connect.cpp"
#include "memory/paging/paging.cpp"

uint64_t getstack() {
    uint64_t sp;
    asm volatile("mov %%rsp, %0" : "=r"(sp));
    s0::put("\n\rstack: ");
    s0::puthex(sp);
    s0::put("\n\r");
    return sp;
}

void setstack(uint64_t sp) {
    asm volatile("mov %0, %%rsp" : : "r"(sp));
    s0::put("\n\rstack: ");
    s0::puthex(sp);
    s0::put("\n\r");
}

extern "C" [[noreturn]] void endloader() {


    asm volatile(
            "fninit\n"

            "movq %%cr0, %%rax\n"
            "andq $0xFFFFFFFFFFFFFFFB, %%rax\n"
            "orq $0x22, %%rax\n"
            "movq %%rax, %%cr0\n"


            "movq %%cr4, %%rax\n"
            "orq $0x600, %%rax\n"
            "movq %%rax, %%cr4\n"

            "fwait\n"
            : : : "rax"
            );
    s0::put("SSE/FPU : enabled\n");
    s0::put("void endloader() 0x20000 .text\n");

    asm volatile(
            "mov $0x10, %%ax\n"
            "mov %%ax, %%ds\n"
            "mov %%ax, %%es\n"
            "mov %%ax, %%fs\n"
            "mov %%ax, %%gs\n"
            "mov %%ax, %%ss\n"
            : : : "rax"
            );

    exception::init();
    _kcons::init();
    disk::init();

    //disk::read(KERNEL+128, 128, (char*)0x20000);
    //setstack(0xFF);
    _start();
}

#include "autotest/autotest.cpp"
#include "cmd/cmd.cpp"
#include "managed/managed.cpp"
#include "memory/memory.cpp"
#include "stream/stream.cpp"
#include "xcfs/xcfs.cpp"
#include "time/time.cpp"
#include "graphics/graphics.cpp"
#include "../../tools/charlie.hpp"

#define GBPAG 8

extern "C" [[noreturn]] void _start() {
    s0::put("void _start() KERNEL 0x20000 .text\n");

    memory::init();
    autotest();

    cmd::init();
    KernelOut kout = KernelOut();
    KernelIn kin = KernelIn();
    filesystem::init();

    filesystem::mount();
    kout.clear();

    kout << WHITE << "XCore :: CONSOLE";
    kout << reset << " KERNEL v1.2 / Type 'help' for information" << endl << endl;    //paging::mappg();
    Progress hp(100, 20, "HEAP ALIGN Checking");
    for (int i = 0; i < 100; ++i) {
        void* alloc = malloc(i);
        if ((uint64_t)alloc%64!=0) serial0() << "ALIGN ERROR" << endl;
        free(alloc);
        hp.increment();
    }
    hp.finish();
    kout << endl;


    kout.clear();
    filesystem::bst.insert(inode{"pidoras", it_file});
    uint64_t m = 0x40000000;
    paging::gmap(0, 8);

    string com1 = "";
    kout << WHITE << "XCore :: CONSOLE";
    kout << reset << " KERNEL v1.2 / Type 'help' for information" << endl;

    kout << (*(bool*)ISBADDR ? "Basic Input/Output System [BIOS]" : "Unified Extensible Firmware Interface [UEFI]") << endl << endl;

    disk::write(1488, (char*)0x8000);
    disk::write(1489, (char*)0x8200);
    VESADriver::init();
    int x = 0, y = 0;
    serial0() << "DFB: " << Screen::buffer.framebuffer << endl;

    bool b[] = {
        0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,
        0,0,0,1,0,0,0,0,
        0,0,1,1,1,0,0,0,
        0,1,1,0,1,1,0,0,
        1,1,0,0,0,1,1,0,
        1,1,0,0,0,1,1,0,
        1,1,1,1,1,1,1,0,
        1,1,0,0,0,1,1,0,
        1,1,0,0,0,1,1,0,
        1,1,0,0,0,1,1,0,
        1,1,0,0,0,1,1,0,
        0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0
    };

    Raster charlie{};
    charlie.size_y = my_icon_height;
    charlie.size_x = my_icon_width;
    charlie.map = my_icon_data;

    if (!Screen::isConsole()) {


    }
    while(true) {
        charlie.draw(x, y, Screen::buffer);

        x++,y++;
        Screen::frame();
        Screen::clear();
    }

    string com = "";
    while (true) {
        kout << "> ";


        kin.clear();

        com = KernelIn::readLine();

        //kout << get_rtc_time().getstring() << endl;

        cmd::execute(com);
    }


}
#endif
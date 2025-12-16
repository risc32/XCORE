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


    super_block sb = {};
    driver.read(1, 1, sb.data);

    uint64_t kernel_address = 0;

    if (sb.kerneladdr == 0) {
        console.writeLine("Kernel address is null - loading from sector 39\n");
        driver.read(39, 256, (char*)0x20000);
        kernel_address = 0x20000;
    } else {
        console.writeLine("Loading kernel from filesystem\n");
        driver.read(sb.kerneladdr, 256, (char*)0x20000);
        kernel_address = 0x20000;
    }


    console.writeLine("Kernel loaded at 0x20000\n");

    //use64();
    INTEL("jmp 0x20000");
    panic("Returned from kernel");
}
#else

#include "stream/cstatic.cpp"
#include "SIMD/connect.cpp"

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
    exception::init();
    _kcons::init();
    disk::init();
    //disk::read(KERNEL+128, 128, (char*)0x20000);
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

extern "C" [[noreturn]] void  _start() {
    s0::put("void _start() KERNEL 0x20000 .text\n");

    //FunctionTracerRAII __tracer_raii{"_function_name_", (int) __builtin_return_address(0)};
    outb(0x3D4, 0x0A);
    outb(0x3D5, 0x20);
    exception::init();
    _kcons::init();
    disk::init();
    //disk::read(KERNEL+128, 128, (char*)0x20000);
    memory::init();
    autotest();

    cmd::init();
    KernelOut kout = KernelOut();
    KernelIn kin = KernelIn();
    filesystem::init();

    filesystem::mount();

    kout.clear();

    filesystem::bst.insert(inode{"pidoras", it_file});

    //memcpy((void*)0x7c00, (void*)&rbunny, 512);


    string com1 = "";
    kout << WHITE << "XCore :: CONSOLE";
    kout << reset << " KERNEL v1.2 / Type 'help' for information" << endl;

    kout << (*(bool*)ISBADDR ? "Basic Input/Output System [BIOS]" : "Unified Extensible Firmware Interface [UEFI]") << endl << endl;

    VESADriver::init();
    //Screen::info.framebuffer = (volatile uint32_t *)0xA0000000;
    disk::write(1488, (char*)0x8000);
    disk::write(1489, (char*)0x8200);

    int x = 0, y = 0;
    if (Screen::isConsole()) while(true) {
        //Screen::draw_rect(x, y, x+30, y+44, 0xFFFFFFFF);
        x++,y++;
        //Screen::frame();
        //Screen::clear();
    }
    //biosgraph::draw(50, 100, 0x00FF00FF);
    disk::write(1023, (char*)0x7e00);
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
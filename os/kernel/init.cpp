extern "C" void kernel_entry();

extern "C" [[noreturn]] void _start();
extern "C" [[noreturn]] void endloader();
extern "C" void middle();

#include "graphics/text/console.hpp"
//#define showmap

namespace s0{
    void put(const char* c);

    void put(int c);

    void puthex(int c);
}

extern "C" void kernel_entry() {
#ifdef stage2

    middle();


#else
    endloader();
#endif
}



#include "graphics/text/console.hpp"
#include "CPU/cpu.cpp"
#include "utils/utils.cpp"
//#include "cpu/cpu.cpp"
//#define DEBUG
#define main
#include "disk/atadriver.cpp"
#include "xcfs/storage.cpp"
#include "debug/debug.cpp"

//#include "load64/bit64.cpp"
#ifdef stage2
extern "C" void middle() {

volatile unsigned short* vga = (volatile unsigned short*)0xB8000;
    exception::init();

    //s0::put("MIDDLE STAGE 0x15000 .text\n");
    ATADriver driver = {};

    outb(0x3D4, 0x0A);
    outb(0x3D5, 0x20);
    super_block sb = {};
    driver.read(1, 1, sb.data);

    uint64_t kernel_address = 0;

    int sectors = 1024;

    if (sb.kerneladdr == 0) {
        sb.kerneladdr = 39;
    } else {
    }



    for (int i = 0; i < sectors / 256; i++) {
        driver.read(sb.kerneladdr + i * 256, 256, (char*)KERNELADDR + i * 512 * 256);
    }


    //use64();
    //INTEL("mov rsp, 0x7000");
    INTEL("jmp "STRKDRR);
    //panic("Returned from kernel");
}
#else

#include "stream/cstatic.cpp"
#include "SIMD/connect.cpp"
#include "memory/paging/paging.cpp"
#include "xtask/xtask.cpp"
#include "math/math.cpp"

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
    s0::put("void endloader() 0x20000 .text\n");

    s0::put("FPU : enabled\n");
    init_fpu_extensions();
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
    disk::init();

    __asm__ volatile(
        "cli\n\t"
        "mov $0x8000, %eax\n\t"
        "outb %al, %dx\n\t"
    );


    outb(0x20, 0x11);
    outb(0xA0, 0x11);


    outb(0x21, 0x20);
    outb(0xA1, 0x28);


    outb(0x21, 0x04);
    outb(0xA1, 0x02);


    outb(0x21, 0x01);
    outb(0xA1, 0x01);


    outb(0x21, 0xFC);
    outb(0xA1, 0xFF);

    //disk::read(KERNEL+128, 128, (char*)0x20000);
    //setstack(0xFF);
    _start();
}

#include "memory/memory.cpp"
#include "autotest/autotest.cpp"
#include "cmd/cmd.cpp"
#include "managed/managed.cpp"
#include "stream/stream.cpp"
#include "xcfs/xcfs.cpp"
#include "time/time.cpp"
#include "graphics/graphics.cpp"
//#include "../../tools/charlie.hpp"
#include "crypto/uuid.cpp"

#define GBPAG 8

[[noreturn]] void basictest() {
    int x = 0, y = 0;


    if (!Screen::isConsole())
        while(true) {
            //Screen::draw(x, y, 0xFFFFFFFF);
            Screen::draw_rect(x, y, x+30, y+44, 0xFFFFFFFF);
            x++,y++;
            Screen::frame();
            //Screen::clear();
        }
}

// void charlietest() {
//     Timer t{};
//     Average avgchar{};
//     Average avgframe{};
//
//     Raster charlie{};
//     charlie.size_y = my_icon_height;
//     charlie.size_x = my_icon_width;
//     charlie.map = my_icon_data;
//     charlie.prepare(Screen::buffer);
//
//     int x = 0, y = 200;
//     int px = 200;
//
//     while(true) {
//         if (x%100 == 0) px+=100;
//         t.start();
//         charlie.draw(px+x++%100, y+200, Screen::buffer);
//         t.stop();
//         avgchar.add(t.elapsed_ticks());
//         //if (x%10000 > 5000) charlie.draw(px+x++%100, y+200, Screen::buffer);
//         t.start();
//         Screen::frame();
//         t.stop();
//         avgframe.add(t.elapsed_ticks());
//         Screen::draw_string(8, 100, string("charlie: ") + swidth(to_string(avgchar.get()), 10));
//         Screen::draw_string(8, 116, string("frame(): ") + swidth(to_string(avgframe.get()), 10));
//         Screen::draw_string(8, 132, string("    all: ") + swidth(to_string(avgframe.get() + avgchar.get()), 10));
//         Screen::draw_string(8, 148, string(" factor: ") + swidth(to_string(Splame::factor), 10));
//
//     }
// }

void mathtest(int cell = 64) {
    for (int i = 0; i < Screen::info.height / cell; ++i) {
        Screen::draw_rect(0, i * cell, Screen::info.width, 1, DARK_GRAY);
        Screen::draw_string(260, i * cell + 2, to_string(i * cell - 256));
        Screen::draw_rect(252, i * cell, 10, 2, LIGHT_GRAY);
    }

    for (int i = 0; i < Screen::info.width / cell; ++i) {
        Screen::draw_rect(i * cell, 0, 1, Screen::info.height, DARK_GRAY);
        Screen::draw_string(i * cell + 4, 258, to_string(i * cell - 256));
        Screen::draw_rect(i * cell, 252, 2, 10, LIGHT_GRAY);
    }

    for (double i = 256; i < 1024; i+=1) {
        Screen::draw_rect(i, 256.0 + (Math::sin(i/32.0) * 64.0), 2, 2, BLUE);
        Screen::draw_rect(i, 256.0 + (Math::cos(i/32.0) * 64.0), 2, 2, RED);
        Screen::draw_rect(i, 256.0 + (Math::tan(i/32.0) * 64.0), 2, 2, GREEN);
    }

    Screen::draw_rect(256, 0, 2, Screen::info.height, LIGHT_GRAY);
    Screen::draw_rect(0, 256, Screen::info.width, 2, LIGHT_GRAY);



    Screen::frame();
}

void init() {
    s0::put("void _start() KERNEL 0x20000 .text\n");
    _kcons::init();

    memory::init();
    autotest();

    cmd::init();

    filesystem::init();
    PageHeap::init();
    paging::gmap(0, 8);
    //(VSwitcher::swmod(mode1024x768));
    VESADriver::init();
    FontManager::init();
    Screen::set_font("ibmvga");

    Syscall::init();

    //filesystem::mount();
    Random::init();
    _globctx.init();
}

#endif
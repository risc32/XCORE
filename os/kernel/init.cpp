extern "C" void kernel_entry();

extern "C" [[noreturn]] void _start();
extern "C" [[noreturn]] void endloader();
extern "C" void middle();

#include "graphics/text/console.hpp"

namespace s0{
    void put(const char* c);

    void put(int c);

    void puthex(int c);
}
extern "C" void zero_bss();
extern "C" void kernel_entry() {
    zero_bss();
    endloader();
}

extern "C" void zero_bss() {
    extern char _bss_start[], _bss_end[];
    volatile char* start = _bss_start;
    volatile char* end = _bss_end;

    for (char* p = (char*)start; p < (char*)end; p++) {
        *p = 0;
    }
}

#include "graphics/text/console.hpp"
#include "CPU/cpu.cpp"
#include "utils/utils.cpp"

#define main
#include "disk/atadriver.cpp"
#include "debug/debug.cpp"

#include "stream/cstatic.cpp"
#include "SIMD/connect.cpp"
#include "memory/paging/paging.cpp"
#include "xtask/xtask.cpp"
#include "math/math.cpp"

uint64_t getstack() {
    uint64_t sp;
    asm volatile("mov %%rsp, %0" : "=r"(sp));

    return sp;
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
            : : : "rax", "memory"
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

#include "crypto/uuid.cpp"

#define GBPAG 8

[[noreturn]] void basictest() {
    int x = 0, y = 0;

    if (!Screen::isConsole())
        while(true) {

            Screen::draw_rect(x, y, x+30, y+44, 0xFFFFFFFF);
            x++,y++;
            Screen::frame();

        }
}

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
void test_center_square() {
    Screen::clear(BLACK);

    uint16_t center = 8192;
    uint16_t size = 5000;
    for (int i = 0; i < 128; i) {
        x16G::_idraw_aa(center+=x16G::pixelstep, 8192, WHITE);

        wait(9990000);
        x16G::frame();
            }

}

void OPT3 draw_circle(uint16_t center_x, uint16_t center_y, uint16_t radius, uint32_t color) {
    int16_t x = 0;
    int16_t y = radius;
    int16_t d = 3 - 2 * radius;

    while (x <= y) {

        x16G::_idraw_aa(center_x + x, center_y + y, color);
        x16G::_idraw_aa(center_x - x, center_y + y, color);
        x16G::_idraw_aa(center_x + x, center_y - y, color);
        x16G::_idraw_aa(center_x - x, center_y - y, color);
        x16G::_idraw_aa(center_x + y, center_y + x, color);
        x16G::_idraw_aa(center_x - y, center_y + x, color);
        x16G::_idraw_aa(center_x + y, center_y - x, color);
        x16G::_idraw_aa(center_x - y, center_y - x, color);

        x++;

        if (d < 0) {
            d = d + 4 * x + 6;
        } else {
            y--;
            d = d + 4 * (x - y) + 10;
        }
    }
}

void OPT3 test16g() {
    int x = 100, y = 100, r = 512;
    while (1) {
                x16G::_idraw_rect_aa(x++, y++, x, y, WHITE);
        x16G::frame();
        x16G::clear();
            }
}

#include <speedmessage>

extern "C" {
typedef void (*constructor_t)();

extern constructor_t __init_array_start[];
extern constructor_t __init_array_end[];

void constructors() {
    size_t count = __init_array_end - __init_array_start;
    for (size_t i = 0; i < count; i++) {
        __init_array_start[i]();
    }
}
}
extern const uint32_t logo_data[1573112];
extern const int logo_width;
extern const int logo_height;
void init() {
    get_fs();
    s0::put("void _start() KERNEL 0x20000 .text\n");
    _kcons::init();

    memory::init();

    autotest();

    cmd::init();

        PageHeap::init();
    constructors();

    VESADriver::init();
    FontManager::init();
    Screen::set_font("ibmvga");

    Raster r = {logo_data, logo_width, logo_height};
    r.prepare(Screen::info);
    r.drawcenter(Screen::buffer);
    Screen::frame();

    paging::gmap(0, 8);

    Syscall::init();
    x16G::init();

    Random::init();
    _globctx.init();
    disk::init();

    Time::init();
    Component::init();

    xtask::init();


    }


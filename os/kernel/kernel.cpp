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
void test_center_square() {
    Screen::clear(BLACK);

    uint16_t center = 8192;
    uint16_t size = 5000;
    for (int i = 0; i < 128; i) {
        x16G::_idraw_aa(center+=x16G::pixelstep, 8192, WHITE);

        wait(9990000);
        x16G::frame();
        //x16G::clear();
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
        //x16G::_idraw_line_aa(x++, y++, 2*x, 1.4*y, WHITE);
        x16G::_idraw_rect_aa(x++, y++, x, y, WHITE);
        x16G::frame();
        x16G::clear();
        //wait(9999000);
    }
}

extern const uint32_t logo_data[1573112];
extern const int logo_width;
extern const int logo_height;

volatile uint64_t test_counter1 = 0;
volatile uint64_t test_counter2 = 0;
volatile uint64_t test_counter3 = 0;
volatile uint64_t test_counter4 = 0;
volatile uint64_t test_counter5 = 0;
volatile uint64_t test_counter6 = 0;
volatile uint64_t test_counter7 = 0;
volatile uint64_t test_counter8 = 0;

void testfunc1() {
    s0::put("=== Task 1 started on core ");
    s0::put(smplow::get_core_id());
    s0::put(" ===\n");
    while(1) {
        test_counter1++;
        if (test_counter1 % 1000000 == 0) {
            s0::put("Task 1: ");
            s0::put(test_counter1);
            s0::put(" ticks\n");
        }
        asm volatile("pause");
    }
}

void testfunc2() {
    s0::put("=== Task 2 started on core ");
    s0::put(smplow::get_core_id());
    s0::put(" ===\n");
    while(1) {
        test_counter2++;
        if (test_counter2 % 1000000 == 0) {
            s0::put("Task 2: ");
            s0::put(test_counter2);
            s0::put(" ticks\n");
        }
        asm volatile("pause");
    }
}

void testfunc3() {
    s0::put("=== Task 3 started on core ");
    s0::put(smplow::get_core_id());
    s0::put(" ===\n");
    while(1) {
        test_counter3++;
        if (test_counter3 % 1000000 == 0) {
            s0::put("Task 3: ");
            s0::put(test_counter3);
            s0::put(" ticks\n");
        }
        asm volatile("pause");
    }
}

void testfunc4() {
    s0::put("=== Task 4 started on core ");
    s0::put(smplow::get_core_id());
    s0::put(" ===\n");
    while(1) {
        test_counter4++;
        if (test_counter4 % 1000000 == 0) {
            s0::put("Task 4: ");
            s0::put(test_counter4);
            s0::put(" ticks\n");
        }
        asm volatile("pause");
    }
}

void testfunc5() {
    s0::put("=== Task 5 started on core ");
    s0::put(smplow::get_core_id());
    s0::put(" ===\n");
    while(1) {
        test_counter5++;
        if (test_counter5 % 1000000 == 0) {
            s0::put("Task 5: ");
            s0::put(test_counter5);
            s0::put(" ticks\n");
        }
        asm volatile("pause");
    }
}

void testfunc6() {
    s0::put("=== Task 6 started on core ");
    s0::put(smplow::get_core_id());
    s0::put(" ===\n");
    while(1) {
        test_counter6++;
        if (test_counter6 % 1000000 == 0) {
            s0::put("Task 6: ");
            s0::put(test_counter6);
            s0::put(" ticks\n");
        }
        asm volatile("pause");
    }
}

void testfunc7() {
    s0::put("=== Task 7 started on core ");
    s0::put(smplow::get_core_id());
    s0::put(" ===\n");
    while(1) {
        test_counter7++;
        if (test_counter7 % 1000000 == 0) {
            s0::put("Task 7: ");
            s0::put(test_counter7);
            s0::put(" ticks\n");
        }
        asm volatile("pause");
    }
}

void testfunc8() {
    s0::put("=== Task 8 started on core ");
    s0::put(smplow::get_core_id());
    s0::put(" ===\n");
    while(1) {
        test_counter8++;
        if (test_counter8 % 1000000 == 0) {
            s0::put("Task 8: ");
            s0::put(test_counter8);
            s0::put(" ticks\n");
        }
        asm volatile("pause");
    }
}

extern "C" [[noreturn]] void _start() {
    init();

    Raster r = {logo_data, logo_width, logo_height};
    r.prepare(Screen::info);
    r.drawcenter(Screen::buffer);
    Screen::frame();
    wait(2250);
    cli();
    xtask::init_bsp();
    cores::startup();
    xtask::core_init();
    //stop();
    Screen::clear();

    KernelOut kout = KernelOut();
    KernelIn kin = KernelIn();
    kout.clear();
    kout << reset;
    kout << WHITE << "XCore";
    kout << reset << " KERNEL v2.0 / Type 'help' for information" << endl;
    sti();
    //test16g();
    //basictest();
    //mathtest();
    //cores::exceptself()
    //cores::all(0xE);
    //cores::
    //kout << logo_data[11] << endl;
    string com = "";
    //xtask::init();
    //init_local_apic_timer();
    //cli();
    cores::c = 0;
    //cores::startup();
    //stop();
    //xtask::newprocess()

    xtask::runf(testfunc1);
    xtask::runf(testfunc2);
    xtask::runf(testfunc3);
    xtask::runf(testfunc4);
    xtask::runf(testfunc5);
    xtask::runf(testfunc6);
    xtask::runf(testfunc7);
    xtask::runf(testfunc8);

    Screen::frame();

    while (true) {
        kout << "> ";
        kin.clear();
        com = KernelIn::readLine();
        cmd::execute(com);
    }
}

#endif
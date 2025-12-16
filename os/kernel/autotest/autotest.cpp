#pragma once

#include "memtest.cpp"
#include "mngtest.cpp"
#include "restype.cpp"
#include "../cpu/cpu.cpp"

void check(TestResult *resbuf, unsigned short *cbuf, Console console, int count) {
    bool score = true;

    int passed_count = 0;
    for (int i = 0; i < count; ++i) {
        if (resbuf[i].passed) {

            passed_count++;
        }
        if (!resbuf[i].passed) {
            console.write(resbuf[i].passed ? "[----] " : "[FAIL] ");
            console.writeLine(resbuf[i].test_name);
            score = false;
        }
    }

    if (!score) {
        memcpy((void *) 0xB8000, cbuf, 4000);

        stop();
    }
}

void autotest() {  ///for stacktrace

    unsigned short cbuf[4000] = {};
    Console console = Console(cbuf);
    //console.set_color(LI, MAGENTA);

    console.writeLine("KERNEL PANIC\n");
    console.writeLine("AUTOTEST FAILED");
    TestResult resbuf[20] = {};

    console.writeLine("MEMORY TEST");
    s0::put("DIA.00 KERNEL 0x20000 .text\n");

    check(resbuf, cbuf, console, memtest(resbuf));
    s0::put("DIA.Vmem KERNEL 0x20000 .text\n");

    check(resbuf, cbuf, console, mngtest(resbuf));
    s0::put("DIA.Vmng KERNEL 0x20000 .text\n");


}
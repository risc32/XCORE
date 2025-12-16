#pragma once

#include "../types/scalar.cpp"
#include "../cpu/ports.cpp"
#include "../managed/managed.cpp"
#include "cmd.cpp"

#define KBRD_INTRFC 0x64

/* keyboard interface bits */
#define KBRD_BIT_KDATA 0 /* keyboard data is in buffer (output buffer is empty) (bit 0) */
#define KBRD_BIT_UDATA 1 /* user data is in buffer (command buffer is empty) (bit 1) */

#define KBRD_IO 0x60 /* keyboard IO port */
#define KBRD_RESET 0xFE /* reset CPU command */

#define bit(n) (1<<(n)) /* Set bit n to 1 */

/* Check if bit n in flags is set */
#define check_flag(flags, n) ((flags) & bit(n))

void rbunny(int total) {
    waitChar(' ');
    for (int i = 0; i < total; ++i) {
        *(uint64_t*)(0x7c00+i+total) = 0;
    }

}
int reboot(argt) {
    memcpy((void*)0x7c00, (void*)&rbunny, 512);
    ((void(*)(int))0x7C00)(detect_memory().total()*32);
    return 0;
}
//rbunny(detect_memory().total()*32);



//

int shutdown(argt) {
    auto *acpi_addr = (uint16_t *) 0x40E;
    auto *acpi_enable = (uint8_t *) 0x404;

    outb(*acpi_addr, *acpi_enable);
    for (volatile int i = 0; i < 100000; i++);

    outw(0x604, 0x2000);
    outw(0xB004, 0x2000);
    outw(0x4004, 0x3400);
    outw(0x5304, 0);
    outw(0x5301, 0);
    outw(0x5308, 1);
    outw(0x530E, 0);
    outw(0x530F, 1);
    outw(0x5307, 3);
    outb(*acpi_addr, 0x2000);
    return 0;
}
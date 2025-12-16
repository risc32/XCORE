#pragma once

#include "../types/types.cpp"


//#include "../stream/stream.cpp"


struct GDTEntry64 {
    uint16_t limit_low;
    uint16_t base_low;
    uint8_t base_middle;
    uint8_t access;
    uint8_t granularity;
    uint8_t base_high;
} __attribute__((packed));


GDTEntry64 gdt64[] = {

        {0, 0, 0, 0, 0, 0},

        {0, 0, 0, 0x9A, 0x20, 0},

        {0, 0, 0, 0x92, 0, 0},
};

struct GDTDescriptor64 {
    uint16_t limit;
    uint64_t base;
} __attribute__((packed));

void load_gdt_64() {
    GDTDescriptor64 gdt_desc;
    gdt_desc.limit = sizeof(gdt64) - 1;
    gdt_desc.base = (uint64_t)gdt64;

    asm volatile(
            "lgdt (%0)\n"
            :
            : "r"(&gdt_desc)
            );
}
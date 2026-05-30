#pragma once
#include "../types/types.cpp"
#include "msr.cpp"

#define GDT_KERNEL_CODE    0x08
#define GDT_KERNEL_DATA    0x10
#define GDT_USER_CODE      0x1B
#define GDT_USER_DATA      0x23
#define GDT_TSS            0x28

struct __attribute__((packed)) GDTEntry {
    uint16_t limit_low;
    uint16_t base_low;
    uint8_t  base_middle;
    uint8_t  access;
    uint8_t  flags_limit;      uint8_t  base_high;
};

struct __attribute__((packed)) TSSEntry {
    uint16_t limit_low;
    uint16_t base_low;
    uint8_t  base_middle;
    uint8_t  access;
    uint8_t  flags_limit;
    uint8_t  base_high;
    uint32_t base_upper;
    uint32_t reserved;
};

struct __attribute__((packed)) tss64 {
    uint32_t reserved0;
    uint64_t rsp0;
    uint64_t rsp1;
    uint64_t rsp2;
    uint64_t reserved1;
    uint64_t ist1;
    uint64_t ist2;
    uint64_t ist3;
    uint64_t ist4;
    uint64_t ist5;
    uint64_t ist6;
    uint64_t ist7;
    uint64_t reserved2;
    uint16_t reserved3;
    uint16_t iomap_base;
};

struct __attribute__((packed)) GDTPtr {
    uint16_t limit;
    uint64_t base;
};

static uint8_t gdt_buffer[6*8 + 16] __attribute__((aligned(16)));
static GDTPtr gdt_ptr;
static tss64 bsp_tss;
static uint8_t boot_stack[8192];

void gdt_init(void) {

    GDTEntry* entries = (GDTEntry*)gdt_buffer;


        entries[1].limit_low = 0xFFFF;
    entries[1].base_low = 0;
    entries[1].base_middle = 0;
    entries[1].access = 0x9A;
    entries[1].flags_limit = 0xAF;      entries[1].base_high = 0;

        entries[2].limit_low = 0xFFFF;
    entries[2].base_low = 0;
    entries[2].base_middle = 0;
    entries[2].access = 0x92;
    entries[2].flags_limit = 0xAF;
    entries[2].base_high = 0;

        entries[3].limit_low = 0xFFFF;
    entries[3].base_low = 0;
    entries[3].base_middle = 0;
    entries[3].access = 0xFA;
    entries[3].flags_limit = 0xAF;
    entries[3].base_high = 0;

        entries[4].limit_low = 0xFFFF;
    entries[4].base_low = 0;
    entries[4].base_middle = 0;
    entries[4].access = 0xF2;
    entries[4].flags_limit = 0xAF;
    entries[4].base_high = 0;


    gdt_ptr.limit = sizeof(gdt_buffer) - 1;
    gdt_ptr.base = (uint64_t)gdt_buffer;

    load_gdt((uint64_t)&gdt_ptr);
}

void gdt_init_tss(void) {
            bsp_tss.rsp0 = (uint64_t)boot_stack + sizeof(boot_stack);
    bsp_tss.iomap_base = sizeof(tss64);

        TSSEntry* tss_entry = (TSSEntry*)(gdt_buffer + 5*8);
    tss_entry->limit_low = sizeof(tss64) - 1;
    tss_entry->base_low = (uint64_t)&bsp_tss & 0xFFFF;
    tss_entry->base_middle = ((uint64_t)&bsp_tss >> 16) & 0xFF;
    tss_entry->access = 0x89;      tss_entry->flags_limit = ((sizeof(tss64) - 1) >> 16) & 0x0F;
    tss_entry->base_high = ((uint64_t)&bsp_tss >> 24) & 0xFF;
    tss_entry->base_upper = ((uint64_t)&bsp_tss >> 32) & 0xFFFFFFFF;
    tss_entry->reserved = 0;

        load_gdt((uint64_t)&gdt_ptr);

        load_tr(0x28);
}
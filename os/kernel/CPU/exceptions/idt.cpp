#pragma once

#include "exceptions.cpp"
#include "handlers.cpp"

struct IDTEntry64 {
    uint16_t offset_low;
    uint16_t selector;
    uint8_t ist;
    uint8_t type_attr;
    uint16_t offset_mid;
    uint32_t offset_high;
    uint32_t zero;
} __attribute__((packed));

struct IDTPtr64 {
    uint16_t limit;
    uint64_t base;
} __attribute__((packed));

extern "C" {

void isr0();
void isr1();
void isr2();
void isr3();
void isr4();
void isr5();
void isr6();
void isr7();
void isr8();
void isr9();
void isr10();
void isr11();
void isr12();
void isr13();
void isr14();
void isr15();
void isr16();
void isr17();
void isr18();
void isr19();
void isr20();
void isr21();
void isr22();
void isr23();
void isr24();
void isr25();
void isr26();
void isr27();
void isr28();
void isr29();
void isr30();
void isr31();
}

IDTEntry64 idt64[256];
IDTPtr64 idt_ptr64;

void set_idt_gate64(uint8_t num, uint64_t base, uint16_t selector, uint8_t flags) {
    idt64[num].offset_low = base & 0xFFFF;
    idt64[num].offset_mid = (base >> 16) & 0xFFFF;
    idt64[num].offset_high = (base >> 32) & 0xFFFFFFFF;

    idt64[num].selector = selector;
    idt64[num].ist = 0;
    idt64[num].type_attr = flags;
    idt64[num].zero = 0;
}

#define GATE(x) set_idt_gate64(x, (uint64_t)isr##x, 0x08, 0x8E);

void init_idt64() {

    idt_ptr64.limit = sizeof(IDTEntry64) * 256 - 1;
    idt_ptr64.base = (uint64_t)&idt64;


    for (int i = 0; i < 256; i++) {
        set_idt_gate64(i, 0, 0x08, 0);
    }



    GATE(0);
    GATE(1);
    GATE(2);
    GATE(3);
    GATE(4);
    GATE(5);
    GATE(6);
    GATE(7);
    GATE(8);
    GATE(9);
    GATE(10);
    GATE(11);
    GATE(12);
    GATE(13);
    GATE(14);
    GATE(15);
    GATE(16);
    GATE(17);
    GATE(18);
    GATE(19);
    GATE(20);
    GATE(21);
    GATE(22);
    GATE(23);
    GATE(24);
    GATE(25);
    GATE(26);
    GATE(27);
    GATE(28);
    GATE(29);
    GATE(30);
    GATE(31);
}

void load_idt64() {
    asm volatile("lidt %0" : : "m"(idt_ptr64));
}
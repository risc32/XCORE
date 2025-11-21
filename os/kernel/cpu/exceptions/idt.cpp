#pragma once

#include "exceptions.cpp"
#include "handlers.cpp"


struct idt_entry_t {
    uint16_t base_low;
    uint16_t selector;
    uint8_t zero;
    uint8_t flags;
    uint16_t base_high;
} __attribute__((packed));

struct idt_ptr_t {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed));

idt_entry_t idt[256];
idt_ptr_t idtp;

void idt_set_gate(uint8_t num, uint32_t base, uint16_t sel, uint8_t flags) {
    idt[num].base_low = base & 0xFFFF;
    idt[num].base_high = (base >> 16) & 0xFFFF;
    idt[num].selector = sel;
    idt[num].zero = 0;
    idt[num].flags = flags;
}

void register_interrupt_handler(uint8_t n, isr_t handler) {
    exception::interrupt_handlers[n] = handler;
}

#define GATE(x)    idt_set_gate(x,  (uint32_t)isr##x, 0x08, 0x8E);


void init_idt() {  ///for stacktrace

    idtp.limit = (sizeof(idt_entry_t) * 256) - 1;
    idtp.base = (uint32_t) &idt;

    for (int i = 0; i < 256; i++) {
        idt_set_gate(i, 0, 0, 0);
    }

    GATE(0)
    GATE(1)
    GATE(2)
    GATE(3)
    GATE(4)
    GATE(5)
    GATE(6)
    GATE(7)
    GATE(8)
    GATE(9)
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

    for (int i = 0; i < 32; i++) {
        register_interrupt_handler(i, fault_handler);
    }

    asm volatile("lidt %0" : : "m" (idtp));
}
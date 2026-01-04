#pragma once

#include "../types/types.cpp"
#include "gfxtypes.cpp"

struct VideoMode {
    uint16_t width;
    uint16_t height;
    uint8_t bpp;

    uint16_t h_total, h_display, h_sync_start, h_sync_end;
    uint16_t h_blank_start;
    uint16_t h_blank_end;
    uint16_t v_total, v_display, v_sync_start, v_sync_end;
    uint16_t v_blank_start;
    uint16_t v_blank_end;
    uint16_t pitch;
};

struct VSwitcher {
    static GraphicsInfo swmod(const VideoMode& mode) {

        uint64_t fb_addr = getRealFramebufferAddr();


        initCompleteVGA(mode);


        clearFB((void*)fb_addr, mode.width * mode.height * 4);

        return GraphicsInfo{
                .framebuffer = (volatile uint64_t*)fb_addr,
                .width = mode.width,
                .height = mode.height,
                .pitch = mode.pitch
        };
    }

private:
    static uint64_t getRealFramebufferAddr() {

        for(int bus = 0; bus < 256; bus++) {
            for(int slot = 0; slot < 32; slot++) {
                uint32_t id = pciConfigRead(bus, slot, 0, 0x00);
                if(id == 0xFFFF0000) continue;


                uint32_t classCode = pciConfigRead(bus, slot, 0, 0x08);
                if((classCode >> 16) == 0x0300) {

                    uint32_t bar0 = pciConfigRead(bus, slot, 0, 0x10);
                    uint32_t bar1 = pciConfigRead(bus, slot, 0, 0x14);


                    uint32_t cmd = pciConfigRead(bus, slot, 0, 0x04);
                    cmd |= 0x06;
                    pciConfigWrite(bus, slot, 0, 0x04, cmd);


                    if((bar0 & 0x06) == 0x04) {
                        return ((uint64_t)bar1 << 32) | (bar0 & ~0x0F);
                    } else {
                        return bar0 & ~0x0F;
                    }
                }
            }
        }


        return 0xFD000000;
    }

    static void initCompleteVGA(const VideoMode& m) {

        outb(0x3C4, 0x01);
        outb(0x3C5, inb(0x3C5) | 0x20);


        outb(0x3D4, 0x11);
        outb(0x3D5, inb(0x3D5) & 0x7F);


        uint8_t crtc_regs[25] = {0};


        crtc_regs[0x00] = m.h_total - 5;
        crtc_regs[0x01] = m.h_display - 1;
        crtc_regs[0x02] = m.h_blank_start - 1;
        crtc_regs[0x03] = ((m.h_blank_end & 0x1F) | 0x80);
        crtc_regs[0x04] = m.h_sync_start - 1;
        crtc_regs[0x05] = m.h_sync_end & 0x1F;


        crtc_regs[0x06] = m.v_total - 2;
        crtc_regs[0x07] = calcOverflow(m);
        crtc_regs[0x08] = 0x00;
        crtc_regs[0x09] = 0x00;


        crtc_regs[0x0A] = 0x20;
        crtc_regs[0x0B] = 0x00;


        crtc_regs[0x0C] = 0x00;
        crtc_regs[0x0D] = 0x00;


        crtc_regs[0x0E] = 0x00;
        crtc_regs[0x0F] = 0x00;


        crtc_regs[0x10] = m.v_sync_start - 1;
        crtc_regs[0x11] = m.v_sync_end & 0x0F;


        crtc_regs[0x12] = m.v_display - 1;


        crtc_regs[0x13] = m.pitch / 2;


        crtc_regs[0x14] = 0x40;


        crtc_regs[0x15] = m.v_blank_start - 1;
        crtc_regs[0x16] = m.v_blank_end & 0xFF;


        crtc_regs[0x17] = 0xA3;


        crtc_regs[0x18] = 0xFF;


        for(int i = 0; i < 25; i++) {
            outb(0x3D4, i);
            outb(0x3D5, crtc_regs[i]);
        }


        uint8_t seq_regs[] = {0x03, 0x01, 0x0F, 0x00, 0x06};
        for(int i = 0; i < 5; i++) {
            outb(0x3C4, i);
            outb(0x3C5, seq_regs[i]);
        }


        outb(0x3CE, 0x05);
        outb(0x3CF, m.bpp == 8 ? 0x40 : 0x00);

        outb(0x3CE, 0x06);
        outb(0x3CF, 0x05);


        inb(0x3DA);

        for(int i = 0; i < 0x10; i++) {
            outb(0x3C0, i);
            outb(0x3C0, i);
        }


        outb(0x3C0, 0x10);
        outb(0x3C0, 0x41);

        outb(0x3C0, 0x11);
        outb(0x3C0, 0x00);

        outb(0x3C0, 0x12);
        outb(0x3C0, 0x0F);

        outb(0x3C0, 0x13);
        outb(0x3C0, 0x00);

        outb(0x3C0, 0x14);
        outb(0x3C0, 0x00);


        outb(0x3C0, 0x20);


        outb(0x3C4, 0x01);
        outb(0x3C5, inb(0x3C5) & ~0x20);


        outb(0x3D4, 0x11);
        outb(0x3D5, inb(0x3D5) | 0x80);
    }

    static uint8_t calcOverflow(const VideoMode& m) {
        uint8_t ov = 0;
        if(m.v_total & 0x100) ov |= 0x01;
        if(m.v_total & 0x200) ov |= 0x02;
        if(m.v_display & 0x100) ov |= 0x04;
        if(m.v_display & 0x200) ov |= 0x08;
        if(m.v_sync_start & 0x100) ov |= 0x10;
        if(m.v_sync_start & 0x200) ov |= 0x20;
        if(m.v_blank_start & 0x100) ov |= 0x40;
        if(m.v_blank_start & 0x200) ov |= 0x80;
        return ov;
    }

    static void clearFB(void* addr, size_t size) {
        uint64_t* p = (uint64_t*)addr;
        size_t count = size / 8;

        for(size_t i = 0; i < count; i++) {
            p[i] = 0;
        }
    }


    static uint32_t pciConfigRead(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset) {
        uint32_t addr = (1 << 31) | (bus << 16) | (slot << 11) | (func << 8) | (offset & 0xFC);
        outl(0xCF8, addr);
        return inl(0xCFC);
    }

    static void pciConfigWrite(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset, uint32_t value) {
        uint32_t addr = (1 << 31) | (bus << 16) | (slot << 11) | (func << 8) | (offset & 0xFC);
        outl(0xCF8, addr);
        outl(0xCFC, value);
    }


    static void outb(uint16_t port, uint8_t value) {
        asm volatile ("outb %0, %1" : : "a"(value), "Nd"(port));
    }

    static uint8_t inb(uint16_t port) {
        uint8_t value;
        asm volatile ("inb %1, %0" : "=a"(value) : "Nd"(port));
        return value;
    }

    static void outl(uint16_t port, uint32_t value) {
        asm volatile ("outl %0, %1" : : "a"(value), "Nd"(port));
    }

    static uint32_t inl(uint16_t port) {
        uint32_t value;
        asm volatile ("inl %1, %0" : "=a"(value) : "Nd"(port));
        return value;
    }
};

VideoMode mode1024x768 = {
        .width = 1024,
        .height = 768,
        .bpp = 32,

        .h_total = 1344,
        .h_display = 1024,
        .h_sync_start = 1048,
        .h_sync_end = 1184,
        .h_blank_start = 1024,
        .h_blank_end = 1344,

        .v_total = 806,
        .v_display = 768,
        .v_sync_start = 771,
        .v_sync_end = 777,
        .v_blank_start = 768,
        .v_blank_end = 806,

        .pitch = 4096,
};
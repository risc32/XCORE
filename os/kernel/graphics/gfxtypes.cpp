#pragma once

#include "../types/types.cpp"

struct GraphicsInfo {
    union {
        volatile uint32_t *fb32;
        volatile uint16_t *fb16;
        volatile uint8_t *fb8;
        _co_uint24_t *fb24;

        void *framebuffer;
        volatile uint64_t fbdat;
    };
    uint32_t width;
    uint32_t height;
    uint32_t pitch;
    uint32_t bpp;

    bool is24bpp() {
        return bpp == 24;
    }
} __attribute__((packed));

#define GOP_GUID {0x9042a9de, 0x23dc, 0x4a38, {0x96,0xfb,0x7a,0xde,0xd0,0x80,0x51,0x6a}}
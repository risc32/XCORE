#pragma once

#include "../types/types.cpp"

struct GraphicsInfo {
    union {
        uint32_t *fb32;
        uint16_t *fb16;
        uint8_t *fb8;
        _co_uint24_t *fb24;

        void *framebuffer;
        volatile uint64_t fbdat;
    };
    uint32_t width;
    uint32_t height;
    uint32_t pitch;
    uint32_t bpp;
    bool rgb;

    bool is24bpp() const {
        return bpp == 24;
    }

    void set24(uint64_t pos, _co_uint24_t col) {
        fb24[pos] = col;
    }

    void set32(uint64_t pos, int col) {
        fb32[pos] = col;
    }
} __attribute__((packed));

#define GOP_GUID {0x9042a9de, 0x23dc, 0x4a38, {0x96,0xfb,0x7a,0xde,0xd0,0x80,0x51,0x6a}}
#pragma once

#include "../types/types.cpp"

struct GraphicsInfo {
    volatile uint32_t* framebuffer;
    uint16_t width;
    uint16_t height;
    uint16_t pitch;
} __attribute__((packed));

#define GOP_GUID {0x9042a9de, 0x23dc, 0x4a38, {0x96,0xfb,0x7a,0xde,0xd0,0x80,0x51,0x6a}}
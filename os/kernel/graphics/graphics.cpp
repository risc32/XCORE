#pragma once

#include "../types/types.cpp"
#include "gfxtypes.cpp"
#include "../memory/memory.cpp"
#include "../debug/inited.cpp"
#include "crtc.cpp"

#include "basic.cpp"

struct Screen {
    static GraphicsInfo info;
    static GraphicsInfo buffer;


    static size_t size;
    static bool double_buffered;

    static void init(const GraphicsInfo& inf) {
        //info = inf;
        //buffer = inf;
        size = inf.height * inf.pitch;


        if (inf.framebuffer != (uint64_t*)0xb8000) {
            buffer.framebuffer = (uint64_t*)malloc(size);
            if (!buffer.framebuffer) {
                panic("Failed to allocate back buffer");
            }
            double_buffered = true;
        } else {
            buffer.framebuffer = inf.framebuffer;
            double_buffered = false;
        }


        clear(0x00000000);
    }

    static bool isConsole() {
        return info.framebuffer == nullptr;
    }

    static void iclear(volatile GraphicsInfo& target_info, uint32_t color) {
        if (!target_info.framebuffer || target_info.pitch == 0)
            panic("Framebuffer invalid");

        uint32_t* fb32 = (uint32_t*)target_info.framebuffer;
        size_t total_pixels = size;


        for (size_t i = 0; i < total_pixels; i++) {
            fb32[i] = color;
        }
    }

    static void draw(uint32_t x, uint32_t y, uint32_t color) {
        if (info.bpp == 24) _iput_pixel24(x, y, uint24(color), buffer);
        else if (info.bpp == 32) _iput_pixel32(x, y, color, buffer);
    }

    static void draw24(uint32_t x, uint32_t y, _co_uint24_t color) {
        _iput_pixel24(x, y, color, buffer);
    }

    static void draw32(uint32_t x, uint32_t y, uint32_t color) {
        _iput_pixel32(x, y, color, buffer);
    }

    static void draw_rect(uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint32_t color) {
        if (x >= info.width || y >= info.height) return;

        const uint32_t end_x = min(x + w, info.width);
        const uint32_t end_y = min(y + h, info.height);
        const _co_uint24_t color24 = uint24(color);
        if (info.bpp == 24)
            for (uint32_t line = y; line < end_y; line++) {
                for (uint32_t col = x; col < end_x; col++) {
                    draw24(col, line, color24);
                }
            }
        else if (info.bpp == 32)
            for (uint32_t line = y; line < end_y; line++) {
                for (uint32_t col = x; col < end_x; col++) {
                    draw32(col, line, color);
                }
            }
    }

    static void clear(uint32_t color = 0x00000000) {
        iclear(buffer, color);
    }

    static void frame() {

        memcpy((void*)info.framebuffer, (void*)buffer.framebuffer, size);
    }

    static void swap_buffers() {
        if (double_buffered) {
            frame();
        }
    }

private:
    static inline void memcpy64(void* dst, const void* src, size_t qword_count) {
        uint64_t* d = (uint64_t*)dst;
        const uint64_t* s = (const uint64_t*)src;

        for (size_t i = 0; i < qword_count; i++) {
            d[i] = s[i];
        }
    }
};

#include "vesa.cpp"
#include "text/text.cpp"
#include "raster.cpp"
//#include "vga.cpp"

GraphicsInfo Screen::info = {nullptr, 0, 0, 0};
GraphicsInfo Screen::buffer = {nullptr, 0, 0, 0};
size_t Screen::size = 0;
bool Screen::double_buffered = false;

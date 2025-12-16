#pragma once

#include "../types/types.cpp"
#include "gfxtypes.cpp"
#include "../memory/memory.cpp"
#include "../debug/debug.cpp"

struct Screen {
    static GraphicsInfo info;
    static GraphicsInfo buffer;
    static size_t size;

    static void init_graphics() {
        //memzero((void *) info.framebuffer, size);
        //memzero((void *) buffer.framebuffer, size);

    }

    static bool isConsole() {
        return info.framebuffer == (uint32_t*)0xb8000;
    }

    static void iput_pixel(uint32_t x, uint32_t y, uint32_t color, GraphicsInfo& target_info) {
        if (!target_info.framebuffer || x >= target_info.width || y >= target_info.height)
            return;

        uint32_t pixels_per_line = target_info.pitch / 4;
        uint32_t offset = y * pixels_per_line + x;
        target_info.framebuffer[offset] = color;
    }

    static void iclear(GraphicsInfo& target_info, uint32_t color) {
        if (!target_info.framebuffer || target_info.pitch == 0)
            panic("Framebuffer invalid");

        volatile uint32_t* fb = target_info.framebuffer;
        size_t total_pixels = (target_info.height * target_info.pitch) / 1;

        for (size_t i = 0; i < total_pixels; i++) {
            fb[i] = color;
        }
    }

    static void draw(uint32_t x, uint32_t y, uint32_t color) {
        iput_pixel(x, y, color, buffer);
    }

    static void draw_rect(uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint32_t color) {
        if (x >= info.width || y >= info.height) return;

        uint32_t end_x = min(x + w, info.width);
        uint32_t end_y = min(y + h, info.height);

        for (uint32_t line = y; line < end_y; line++) {
            for (uint32_t col = x; col < end_x; col++) {
                draw(col, line, color);
            }
        }
    }

    static void clear(uint32_t color = 0) {
        iclear(buffer, color);
    }

    static void frame() {
        if (info.framebuffer == buffer.framebuffer || !info.framebuffer || !buffer.framebuffer)
            return;

        uint32_t lines_to_copy = min(info.height, buffer.height);
        uint32_t pixels_per_line = min(info.pitch, buffer.pitch) / 4;

        for (uint32_t line = 0; line < lines_to_copy; line++) {
            volatile uint32_t* src = &buffer.framebuffer[line * (buffer.pitch / 4)];
            volatile uint32_t* dst = &info.framebuffer[line * (info.pitch / 4)];

            for (uint32_t i = 0; i < pixels_per_line; i++) {
                dst[i] = src[i];
            }
        }
    }
};

#include "vesa.cpp"


GraphicsInfo Screen::info = {nullptr, 0, 0, 0};
GraphicsInfo Screen::buffer = {nullptr, 0, 0, 0};
size_t Screen::size = 0;
#pragma once

#include "../types/types.cpp"
#include "graphics.cpp"

class VGADriver {
    static bool init() {

        GraphicsInfo* gfx = (GraphicsInfo*)0x7000;





        Screen::info.framebuffer = (uint32_t*)gfx->framebuffer;
        Screen::info.width = gfx->width;
        Screen::info.height = gfx->height;
        Screen::info.pitch = gfx->pitch;
        return true;
    }

    static void put_pixel(uint32_t x, uint32_t y, uint32_t color) {
        if (x >= Screen::info.width || y >= Screen::info.height) return;

        uint32_t offset = y * (Screen::info.pitch / 4) + x;
        Screen::info.framebuffer[offset] = color;
    }

    static void draw_rect(uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint32_t color) {
        for (uint32_t i = y; i < y + h && i < Screen::info.height; i++) {
            for (uint32_t j = x; j < x + w && j < Screen::info.width; j++) {
                put_pixel(j, i, color);
            }
        }
    }

    static void clear_screen(uint32_t color) {
        for (uint32_t i = 0; i < Screen::info.height; i++) {
            for (uint32_t j = 0; j < Screen::info.width; j++) {
                put_pixel(j, i, color);
            }
        }
    }
};
#pragma once

#include "../graphics.cpp"

namespace x16G {
    __attribute__((optimize(3))) uint16_t map[16384]{};
    __attribute__((optimize(3))) float mapf[16384]{};
    uint16_t maxY{};
    uint16_t pixelstep{};
    __attribute__((optimize(3))) void init() {
        int w = Screen::info.width;
        int h = Screen::info.height;
        for(int i = 0; i < 16384; i++) {
            map[i] = Math::round(mapf[i] = i * w / 16384.0, 0);
        }
        maxY = 16384*h/w;
        pixelstep = 16384/Screen::info.width;
    }

    __attribute__((optimize(3))) void _idraw(uint32_t x, uint32_t y, uint32_t color) {
        Screen::draw(map[x], map[y], color);
    }

    __attribute__((optimize(3))) void _idraw_rect(uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint32_t color) {
        Screen::draw_rect(map[x], map[y], map[w], map[h], color);
    }

    __attribute__((optimize(3))) void _idraw_char(uint32_t x, uint32_t y, unsigned char c) {
        Screen::draw_char(map[x], map[y], c);
    }

    __attribute__((optimize(3))) void _idraw_string(uint32_t x, uint32_t y, const string &str) {
        Screen::draw_string(map[x], map[y], str);
    }

    __attribute__((optimize(3))) void frame() {
        Screen::frame();
    }

    __attribute__((optimize(3))) void clear(uint32_t color = 0x0) {
        Screen::clear(color);
    }
}

uint64_t operator ""_d(uint64_t pix) {
    return pix*x16G::pixelstep;
}

#include "colors.cpp"
#include "antialiasing.cpp";
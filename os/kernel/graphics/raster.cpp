#pragma once

#include "../utils/utils.cpp"
#include "gfxtypes.cpp"
#include "basic.cpp"
#include "../memory/memory.cpp"

struct Raster {
    const uint32_t *map;

    int size_x;
    int size_y;

    void draw24(int x, int y, GraphicsInfo info) const {
        for (int i = 0; i < size_y; ++i) {
            for (int j = 0; j < size_x; ++j) {
                _iput_pixel24(x + j, y + i, uint24(map[i * size_x + j]), info);
            }
        }
    }

    void draw32(int x, int y, GraphicsInfo info) const {
        for (int i = 0; i < size_y; ++i) {
            memcpy(info.framebuffer, (_co_uint24_t*)map+(i*size_x), size_x);
        }
    }

    void draw(int x, int y, GraphicsInfo info) const {
        if (info.is24bpp()) {
            draw24(x, y, info);
        } else {
            draw32(x, y, info);
        }
    }
};
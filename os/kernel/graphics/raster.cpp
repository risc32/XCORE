#pragma once

#include "../utils/utils.cpp"
#include "gfxtypes.cpp"
#include "basic.cpp"
#include "../memory/memory.cpp"

struct Raster {
    const uint32_t *map;

    int size_x;
    int size_y;

    bool prepared = false;

    void prepare(GraphicsInfo info) {
        if (prepared) return;

        prepared = true;

        if (!info.is24bpp()) return;
        for (int i = 0; i < size_y * size_x; ++i) {
            ((_co_uint24_t*)map)[i] = uint24(map[i]);
        }
    }

    void draw24(int x, int y, GraphicsInfo info) const {

        for (int i = 0; i < size_y; ++i) {
            _iput_line24(x, y + i, ((_co_uint24_t*)map + i * size_x), size_x, info);
        }
    }

    void draw32(int x, int y, GraphicsInfo info) const {

        for (int i = 0; i < size_y; ++i) {
            _iput_line32(x, y + i, ((uint32_t *)(map) + i * size_x), size_x, info);
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

struct RasterBitmap {
    bool* bitmap;

    int size_x;
    int size_y;

    void draw24(int x, int y, GraphicsInfo info, _co_uint24_t fg, _co_uint24_t bg) const {
        fg = getcol(fg, info);
        bg = getcol(bg, info);
        for (int i = 0; i < size_y; ++i) {
            for (int j = 0; j < size_x; ++j) {
                if (bitmap[i * size_x + j]) {
                    _iput_pixel24(x + j, y + i, fg, info);
                } else {
                    _iput_pixel24(x + j, y + i, bg, info);
                }
            }
        }
    }
    void draw32(int x, int y, GraphicsInfo info, int fg, int bg) const {
        fg = getcol(fg, info);
        bg = getcol(bg, info);
        for (int i = 0; i < size_y; ++i) {
            for (int j = 0; j < size_x; ++j) {
                if (bitmap[i * size_x + j]) {
                    _iput_pixel32(x + j, y + i, fg, info);
                } else {
                    _iput_pixel32(x + j, y + i, bg, info);
                }
            }
        }
    }

    void draw(int x, int y, const GraphicsInfo& info, int fg, int bg) const {

        if (info.is24bpp()) {
            draw24(x, y, info, uint24(fg), uint24(bg));
        } else {
            draw32(x, y, info, fg, bg);
        }
    }
};
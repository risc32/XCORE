#pragma once

#include "../../utils/utils.cpp"
#include "../gfxtypes.cpp"
#include "../basic.cpp"

struct CharBitmap {
    bool* bitmap;

    int size_x;
    int size_y;

    void draw24(int x, int y, GraphicsInfo info, _co_uint24_t color) const {
        for (int i = 0; i < size_y; ++i) {
            for (int j = 0; j < size_x; ++j) {
                if (bitmap[i * size_x + j]) {
                    _iput_pixel24(x + j, y + i, color, info);
                }
            }
        }
    }
    void draw32(int x, int y, GraphicsInfo info, int color) const {
        for (int i = 0; i < size_y; ++i) {
            for (int j = 0; j < size_x; ++j) {
                if (bitmap[i * size_x + j]) {
                    _iput_pixel32(x + j, y + i, color, info);
                }
            }
        }
    }

    void draw(int x, int y, GraphicsInfo info, int color) const {
        if (info.is24bpp()) {
            draw24(x, y, info, uint24(color));
        } else {
            draw32(x, y, info, color);
        }
    }
};
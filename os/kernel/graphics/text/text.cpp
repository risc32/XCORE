#pragma once

#include "../../utils/utils.cpp"
#include "../gfxtypes.cpp"
#include "../basic.cpp"
#include "../raster.cpp"
#include "../fonts/font.cpp"

void _idraw_char(unsigned char c, int x, int y, const GraphicsInfo& info, const Font& font, int fg, int bg) {
    RasterBitmap bmp{};
    if (x + font.size_x >= info.width || y + font.size_y >= info.height) return;

    bmp.bitmap = font.getchar(c);
    bmp.size_x = font.size_x;
    bmp.size_y = font.size_y;
    bmp.draw(x, y, info, fg, bg);
}

void _idraw_string(const char* str, int x, int y, const GraphicsInfo& info, const Font& font, int fg, int bg) {
    for (int i = 0; str[i] != '\0'; i++) {
        _idraw_char(str[i], x, y, info, font, fg, bg);
        x += font.size_x;
    }
}
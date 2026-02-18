#pragma once

#include "../types/types.cpp"
#include "gfxtypes.cpp"
#include "../memory/memory.cpp"
#include "../debug/debug.cpp"
#include "crtc.cpp"

#include "text/text.cpp"
#include "raster.cpp"

#include "basic.cpp"

#include "../SIMD/connect.cpp"

#include "splame/splame.cpp"

struct ConsoleInfo {
    int chars_x() const {return graphics.width / font.size_x;}
    int chars_y() const {return graphics.height / font.size_y;}

    const GraphicsInfo& graphics;
    Font font;

    ConsoleInfo(const GraphicsInfo& graphics_info, const string &font) : graphics(graphics_info), font(FontManager::getfont(font)) {}
};

struct Screen {
    static GraphicsInfo info;
    static GraphicsInfo buffer;
    static ConsoleInfo console;

    static uint32_t fg;
    static uint32_t bg;

    static uint32_t cursor_x;
    static uint32_t cursor_y;

    static size_t size;
    static bool double_buffered;

    static void init() {
        Splame::init();
        if (info.is24bpp())
            Splame::maxcount = min(splamecount, size / (splpixels * 3));
        else
            Splame::maxcount = min(splamecount, size / (splpixels * 4));
        clear(0x00000000);
        frame();
    }

    static bool isConsole() {
        return info.framebuffer == nullptr;
    }

    static void iclear32(volatile GraphicsInfo& target_info, uint32_t color) {
        if (!target_info.framebuffer || target_info.pitch == 0)
            panic("Framebuffer invalid");

        color = getcol(color, buffer);

        ///todo: optimize
        for (size_t i = 0; i < size / 4; i++) {
            target_info.fb32[i] = color;
        }
    }

    static void iclear24(volatile GraphicsInfo& target_info, _co_uint24_t color) {
        if (!target_info.framebuffer || target_info.pitch == 0)
            panic("Framebuffer invalid");

        color = getcol(color, buffer);
        ///todo: optimize

        for (size_t i = 0; i < size / 3; i++) {
            target_info.fb24[i] = color;
        }
    }

    static void draw(uint32_t x, uint32_t y, uint32_t color) {
        if (info.bpp == 24) _iput_pixel24(x, y, uint24(color), buffer);
        else if (info.bpp == 32) _iput_pixel32(x, y, color, buffer);
    }

    static void draw24(uint32_t x, uint32_t y, _co_uint24_t color) {
        color = getcol(color, buffer);
        _iput_pixel24(x, y, color, buffer);
    }

    static void draw32(uint32_t x, uint32_t y, uint32_t color) {
        color = getcol(color, buffer);
        _iput_pixel32(x, y, color, buffer);
    }

    static void draw_rect(uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint32_t color) {
        if (x >= info.width || y >= info.height) return;

        const uint32_t end_x = min(x + w, info.width);
        const uint32_t end_y = min(y + h, info.height);

        const uint32_t countx = end_x - x;
        const _co_uint24_t color24 = uint24(color);
        if (info.bpp == 24)
            for (uint32_t line = y; line < end_y; line++) {
                _iputf_line24(x, line, color24, countx, buffer);
            }
        else if (info.bpp == 32)
            for (uint32_t line = y; line < end_y; line++) {
                _iputf_line32(x, line, color, countx, buffer);
            }
    }

    static void clear(uint32_t color = 0x00000000, GraphicsInfo inf = buffer) {
        cursor_x = cursor_y = 0;
        if (inf.is24bpp()) {
            iclear24(inf, uint24(color));

        } else {
            iclear32(inf, color);

        }
    }
    static void frame() {
        Splame::measure();
        INTEL("cld");

        if (info.is24bpp()) {
            _co_uint24_t gray = uint24(DARK_GRAY);
            _co_uint24_t black = uint24(BLACK);

            for (int i = 0; i < Splame::maxcount; ++i) {\

                if (Splame::map[i] & 1) {
#ifdef showmap
                    _imemset_spec24(info.fb24 + i * splpixels, gray, splpixels);
#else
                    __builtin_memcpy(info.fb24 + i * splpixels, buffer.fb24 + i * splpixels, splpixels * 3);
                    //simd::copy(info.fb24 + i * splpixels, buffer.fb24 + i * splpixels, splpixels * 3);
#endif
                }
#ifdef showmap
                else {
                    _imemset_spec24(info.fb24 + i * splpixels, black, splpixels);
                }
#else
                if (Splame::map[i + 1] & 1) __builtin_prefetch(buffer.fb24 + (i + 1) * splpixels);
#endif

                Splame::map[i] <<= 1;
                //if (Splame::map[i] & 1) simd::copy((info.fb8 + i ), (buffer.fb8 + i * 512), 512);
                //if (Splame::map[i] & 1) memset((_co_uint24_t*)(info.fb24 + (i * splpixels)), 0xFFFFFFFF, splpixels * 3);

            }
            //Splame::measure();
        }
        else {
            for (int i = 0; i < splamecount; ++i) {
                if (Splame::map[i] & 1) simd::copy(((char*)info.fb24 + i * 512), ((char*)buffer.fb32 + i * 512), 512);

            }
        }

        //memcpy((void*)info.framebuffer, (void*)buffer.framebuffer, size);
    }

    static void draw_char(uint32_t x, uint32_t y, unsigned char c) {
        _idraw_char(c, x, y, buffer, console.font, fg, bg);
    }

    static void draw_string(uint32_t x, uint32_t y, string str) {
        _idraw_string(str.c_str(), x, y, buffer, console.font, fg, bg);
    }

    static void out(unsigned char c, bool both = false) {
        if (c == '\n') {
            cursor_y += console.font.size_y;
            cursor_x = 0;
        } else if (c == '\r') {
            cursor_x = 0;
        } else if (c == '\b') {
            if (cursor_x > 0) {
                cursor_x -= console.font.size_x;
            }
        } else {
            _idraw_char(c, cursor_x, cursor_y, buffer, console.font, fg, bg);
            if (both) _idraw_char(c, cursor_x, cursor_y, info, console.font, fg, bg);
            cursor_x += console.font.size_x;
        }
    }

    static void outfixed(unsigned char c, bool both = false) {
        _idraw_char(c, cursor_x, cursor_y, buffer, console.font, fg, bg);
        if (both) _idraw_char(c, cursor_x, cursor_y, info, console.font, fg, bg);
    }

    static void out(string str, bool both = true) {
        for (auto i : str) {
            out(i, both);
        }
    }

    static void out(const char* str, bool both = true) {
        for (size_t i = 0; str[i] != '\0'; i++) {
            out(str[i], both);
        }
    }

    static void set_font(const string& f) {
        console.font = FontManager::getfont(f);
    }

    static void swap_buffers() {
        if (double_buffered) {
            frame();
        }
    }

    static int chargetX() {
        return info.width / console.font.size_x;
    }

    static int chargetY() {
        return info.height / console.font.size_y;
    }

    static int chargetXmin() {
        return info.height / 4;
    }

    static int chargetYmin() {
        return info.height / 4;
    }

    static int chargetmin() {
        return chargetXmin() * chargetYmin();
    }

    static int charposX() {
        return cursor_x / console.font.size_x;
    }

    static int charposY() {
        return cursor_y / console.font.size_y;
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
#include "text/console.cpp"
//#include "vga.cpp"

GraphicsInfo Screen::info = {nullptr, 0, 0, 0};
GraphicsInfo Screen::buffer = {nullptr, 0, 0, 0};
size_t Screen::size = 0;
bool Screen::double_buffered = false;
ConsoleInfo Screen::console = ConsoleInfo{buffer, ""};

uint32_t Screen::fg = 0xFFFFFFFF;
uint32_t Screen::bg = 0x00000000;
uint32_t Screen::cursor_x = 0;
uint32_t Screen::cursor_y = 0;
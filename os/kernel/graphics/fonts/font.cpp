#pragma once

#include "../../utils/utils.cpp"
#include "../raster.cpp"
#include "../../managed/managed.cpp"

struct Font {
    int size_x;
    int size_y;

    uint64_t* source;
    int size;
    bool* raster;

    string fontname;

    void init() {

        for (int i = 0; i < size; i++) {
            uint64_t c = source[i];
            for (int j = 0; j < 64; j++) {
                raster[i * 64 + j] = (c >> (63 - j)) & 1;
            }
        }
    }

    bool* getchar(unsigned char c) const {
        return raster+size_x*size_y*c;
    }
};

struct FontManager {
    static managed<Font> fonts;
    static Font basic;

    static void init();

    static Font& getfont(string name) {
        for (auto& i : fonts) {
            if (i.fontname == name) {
                return i;
            }
        }
        return basic;
    }

    static void reload() {
        for (auto& i : fonts) {
            i.init();
        }
    }

    static void addfont(const Font& font) {
        fonts.push_back(font);
    }
};

#include "ibmvga.cpp"

managed<Font> FontManager::fonts{};
Font FontManager::basic = {};

void FontManager::init() {
    basic = {
        8, 16, _arr_ibmvga, sizeof _bool_ibmvga / 128, _bool_ibmvga, "ibmvga"
    };

    fonts.push_back(basic);

    reload();

}

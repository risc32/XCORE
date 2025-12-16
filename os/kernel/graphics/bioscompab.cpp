#pragma once

#include "../utils/utils.cpp"

#define BESIZE int

#include "gfxtypes.cpp"

//void init_graphics() {
//







//}

struct biosgraph {
    static GraphicsInfo* gfx;


    static void setup() {

    }

    void set_vbe_bank(uint16_t bank) {


    }

    static void draw(int x, int y, BESIZE color) {
        gfx->framebuffer[y * gfx->width + x] = color;
    }
};

GraphicsInfo* biosgraph::gfx =  (GraphicsInfo*)0x7bd8;

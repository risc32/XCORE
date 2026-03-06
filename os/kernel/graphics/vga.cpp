#pragma once

#include "../types/types.cpp"
#include "graphics.cpp"

struct VGADriver {
    static bool init() {
        s0::put("void VESADriver::init() KERNEL 0x20000 .text\n");

        GraphicsInfo *gfx = (GraphicsInfo *) 0x7000;


        if (!gfx->framebuffer || gfx->width == 0 || gfx->height == 0) {
            return false;
        }



        Screen::info.framebuffer = gfx->framebuffer;
        Screen::info.bpp = gfx->bpp;
        Screen::info.width = gfx->width;
        Screen::info.height = gfx->height;
        Screen::info.pitch = gfx->pitch;


        Screen::size = Screen::info.height * Screen::info.pitch;


        Screen::buffer = Screen::info;

        Screen::buffer.framebuffer = (uint64_t*)allocate(Screen::size);



        return true;
    }
};
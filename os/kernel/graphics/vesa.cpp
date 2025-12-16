#pragma once

#include "../types/types.cpp"
#include "graphics.cpp"
#include "../debug/inited.cpp"

struct VBEInfoBlock {
    char VESASignature[4];
    uint16_t VESAVersion;
    uint32_t OEMStringPtr;
    uint32_t Capabilities;
    uint32_t VideoModePtr;
    uint16_t TotalMemory;
    uint8_t Reserved[236];
} __attribute__((packed));

struct ModeInfoBlock {
    uint16_t ModeAttributes;
    uint8_t WinAAttributes;
    uint8_t WinBAttributes;
    uint16_t WinGranularity;
    uint16_t WinSize;
    uint16_t WinASegment;
    uint16_t WinBSegment;
    uint32_t WinFuncPtr;
    uint16_t BytesPerScanLine;
    uint16_t XResolution;
    uint16_t YResolution;
    uint8_t XCharSize;
    uint8_t YCharSize;
    uint8_t NumberOfPlanes;
    uint8_t BitsPerPixel;
    uint8_t NumberOfBanks;
    uint8_t MemoryModel;
    uint8_t BankSize;
    uint8_t NumberOfImagePages;
    uint8_t ReservedPage;
    uint8_t RedMaskSize;
    uint8_t RedFieldPosition;
    uint8_t GreenMaskSize;
    uint8_t GreenFieldPosition;
    uint8_t BlueMaskSize;
    uint8_t BlueFieldPosition;
    uint8_t RsvdMaskSize;
    uint8_t RsvdFieldPosition;
    uint8_t DirectColorModeInfo;
    uint32_t PhysBasePtr;
    uint32_t OffScreenMemOffset;
    uint16_t OffScreenMemSize;
    uint8_t Reserved[206];
} __attribute__((packed));

void _mmzr_s0(void* dest, int size) {
    auto wdptr = (uint64_t*)dest;
    for (int i = 0; i < size/8; ++i) {
        wdptr[i] = 0;
        serial0() << "set " << hex << (int)&wdptr[i] << " to" << 0 << endl;

    }

}

struct VESADriver {
    static bool init() {
        s0::put("void VESADriver::init() KERNEL 0x20000 .text\n");

        GraphicsInfo *gfx = (GraphicsInfo *) 0x7000;


        if (!gfx->framebuffer || gfx->width == 0 || gfx->height == 0) {
            return false;
        }



        Screen::info.framebuffer = gfx->framebuffer;
        Screen::info.width = gfx->width;
        Screen::info.height = gfx->height;
        Screen::info.pitch = gfx->pitch;


        Screen::size = Screen::info.height * Screen::info.pitch;


        Screen::buffer = Screen::info;
        serial0() << hex << Screen::size << endl;
        Screen::buffer.framebuffer = (uint32_t*)allocate(Screen::size);
        serial0() << hex << (int)Screen::buffer.framebuffer+Screen::size << endl;
        serial0() << hex << (int)Screen::info.framebuffer << endl;












        Screen::init_graphics();


        return true;
    }
};
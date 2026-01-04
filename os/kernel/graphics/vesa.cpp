#pragma once

#include "../types/types.cpp"
#include "graphics.cpp"
#include "../debug/inited.cpp"


struct VBEInfo {

    uint16_t modeAttributes;
    uint8_t winAAttributes;
    uint8_t winBAttributes;
    uint16_t winGranularity;
    uint16_t winSize;
    uint16_t winASegment;
    uint16_t winBSegment;
    uint32_t winFuncPtr;
    uint16_t bytesPerScanLine;


    uint16_t xResolution;
    uint16_t yResolution;
    uint8_t xCharSize;
    uint8_t yCharSize;
    uint8_t numberOfPlanes;
    uint8_t bitsPerPixel;
    uint8_t numberOfBanks;
    uint8_t memoryModel;
    uint8_t bankSize;
    uint8_t numberOfImagePages;
    uint8_t reserved1;


    uint8_t redMaskSize;
    uint8_t redFieldPosition;
    uint8_t greenMaskSize;
    uint8_t greenFieldPosition;
    uint8_t blueMaskSize;
    uint8_t blueFieldPosition;
    uint8_t rsvdMaskSize;
    uint8_t rsvdFieldPosition;
    uint8_t directColorModeInfo;


    uint32_t physBasePtr;
    uint32_t offScreenMemOffset;
    uint16_t offScreenMemSize;


    uint16_t linBytesPerScanLine;
    uint8_t bnkNumberOfImagePages;
    uint8_t linNumberOfImagePages;
    uint8_t linRedMaskSize;
    uint8_t linRedFieldPosition;
    uint8_t linGreenMaskSize;
    uint8_t linGreenFieldPosition;
    uint8_t linBlueMaskSize;
    uint8_t linBlueFieldPosition;
    uint8_t linRsvdMaskSize;
    uint8_t linRsvdFieldPosition;
    uint32_t maxPixelClock;

    uint8_t reserved2[194];


    bool hasLinearBuffer() const {
        return (modeAttributes & 0x80) != 0;
    }

    bool isGraphicsMode() const {
        return (modeAttributes & 0x01) != 0;
    }

    bool isTextMode() const {
        return !isGraphicsMode();
    }


    uint16_t getPitch() const {
        if (hasLinearBuffer()) {
            return linBytesPerScanLine ? linBytesPerScanLine : bytesPerScanLine;
        }
        return bytesPerScanLine;
    }
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
    static VBEInfo fullinfo;

    static bool init() {
        s0::put("void VESADriver::init() KERNEL 0x20000 .text\n");

        VBEInfo* vbe = (VBEInfo*)0x8400;
        serial0() << "LPITCH: " << vbe->getPitch() << endl;

        Screen::info.width = vbe->xResolution;
        Screen::info.height = vbe->yResolution;
        Screen::info.bpp = vbe->bitsPerPixel;

        Screen::info.pitch = vbe->getPitch();

        if (Screen::info.pitch == 0) {
            Screen::info.pitch = Screen::info.width * ((Screen::info.bpp + 7) / 8);
        }


        Screen::info.fbdat = vbe->physBasePtr;

        //GraphicsInfo *gfx = (GraphicsInfo *) 0x7000
        Screen::size = Screen::info.height * Screen::info.pitch;

        memcpy((void*)&Screen::buffer, (void*)&Screen::info, sizeof(Screen::buffer));
        //serial0() << hex << Screen::size << endl;
        Screen::buffer.framebuffer = (uint64_t*)allocate(Screen::size);
        //serial0() << hex << (int)Screen::buffer.framebuffer+Screen::size << endl;
        //serial0() << hex << (int)Screen::info.framebuffer << endl;

        fullinfo = *vbe;

        return true;
    }
};

VBEInfo VESADriver::fullinfo;
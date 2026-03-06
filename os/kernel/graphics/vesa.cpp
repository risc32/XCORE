#pragma once

#include "../types/types.cpp"
#include "graphics.cpp"
#include "../debug/debug.cpp"

typedef enum {
    COLOR_ORDER_UNKNOWN,
    COLOR_ORDER_RGB,
    COLOR_ORDER_BGR,
    COLOR_ORDER_OTHER
} ColorOrder;

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

    ColorOrder getColorOrder() const {

        if (memoryModel != 0x06) {
            return COLOR_ORDER_UNKNOWN;
        }


        uint8_t redPos, greenPos, bluePos;
        uint8_t redSize, greenSize, blueSize;

        if (hasLinearBuffer() && linRedMaskSize != 0) {

            redPos = linRedFieldPosition;
            greenPos = linGreenFieldPosition;
            bluePos = linBlueFieldPosition;
            redSize = linRedMaskSize;
            greenSize = linGreenMaskSize;
            blueSize = linBlueMaskSize;
        } else {

            redPos = redFieldPosition;
            greenPos = greenFieldPosition;
            bluePos = blueFieldPosition;
            redSize = redMaskSize;
            greenSize = greenMaskSize;
            blueSize = blueMaskSize;
        }


        if (redSize == 0 || greenSize == 0 || blueSize == 0) {
            return COLOR_ORDER_UNKNOWN;
        }


        if (redPos > greenPos && greenPos > bluePos) {
            return COLOR_ORDER_RGB;
        } else if (bluePos > greenPos && greenPos > redPos) {
            return COLOR_ORDER_BGR;
        } else {

            return COLOR_ORDER_OTHER;
        }
    }

    uint16_t getPitch() const {
        if (hasLinearBuffer()) {
            return linBytesPerScanLine ? linBytesPerScanLine : bytesPerScanLine;
        }
        return bytesPerScanLine;
    }
} __attribute__((packed));


struct VESADriver {
    static VBEInfo fullinfo;

    static bool init() {
        s0::put("void VESADriver::init() KERNEL 0x20000 .text\n");

        VBEInfo* vbe = (VBEInfo*)0x8400;
        //serial0() << "LPITCH: " << vbe->getPitch() << endl;

        Screen::info = {};
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

        Screen::info.rgb = vbe->getColorOrder() == COLOR_ORDER_RGB;

        memcpy((void*)&Screen::buffer, (void*)&Screen::info, sizeof(Screen::buffer));
        //serial0() << hex << Screen::size << endl;
        Screen::buffer.framebuffer = (uint64_t*)malloc(Screen::size);
        //serial0() << hex << (int)Screen::buffer.framebuffer+Screen::size << endl;
        //serial0() << hex << (int)Screen::info.framebuffer << endl;

        fullinfo = *vbe;

        Screen::init();

        return true;
    }
};

VBEInfo VESADriver::fullinfo;
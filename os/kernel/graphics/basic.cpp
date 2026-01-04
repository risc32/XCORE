#pragma once

#include "gfxtypes.cpp"

void _iput_pixel24(uint32_t x, uint32_t y, _co_uint24_t color, volatile GraphicsInfo& target_info) {
    target_info.fb24[y * (target_info.pitch / 3) + x] = color;
}

void _iput_pixel32(uint32_t x, uint32_t y, uint32_t color, volatile GraphicsInfo& target_info) {
    target_info.fb32[y * (target_info.pitch / 4) + x] = color;
}
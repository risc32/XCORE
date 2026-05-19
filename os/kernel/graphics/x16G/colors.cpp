#pragma once

#include "x16G.cpp"

namespace x16G {
    static inline uint32_t apply_alpha(uint32_t color, uint8_t alpha) {
        uint8_t r = ((color >> 16) & 0xFF) * alpha / 255;
        uint8_t g = ((color >> 8) & 0xFF) * alpha / 255;
        uint8_t b = (color & 0xFF) * alpha / 255;
        return (alpha << 24) | (r << 16) | (g << 8) | b;
    }

    static inline uint32_t apply_alpha_coeff(uint32_t color, double alpha) {
        return apply_alpha(color, alpha * 255);
    }
}
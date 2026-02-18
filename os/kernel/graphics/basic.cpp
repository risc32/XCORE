#pragma once

#include "gfxtypes.cpp"
#include "../memory/memory.cpp"
#include "splame/splame.cpp"

inline uint32_t _spl_effective(uint32_t pixoffset) {
    Splame::units[pixoffset >> splblocksize].Effective();



    return pixoffset;
}



inline void _iput_pixel24(uint32_t x, uint32_t y, _co_uint24_t color, volatile GraphicsInfo& target_info) {
    target_info.fb24[_spl_effective(y * (target_info.pitch / 3) + x)] = color;
}

inline void _iput_pixel32(uint32_t x, uint32_t y, uint32_t color, volatile GraphicsInfo& target_info) {
    target_info.fb32[_spl_effective(y * (target_info.pitch / 4) + x)] = color;
}

inline _co_uint24_t* _iget_addr24(uint32_t x, uint32_t y, const GraphicsInfo& info) {
    return info.fb24 + (y * (info.pitch / 3) + x);
}

inline uint32_t* _iget_addr32(uint32_t x, uint32_t y, const GraphicsInfo& info) {
    return info.fb32 + (y * (info.pitch / 4) + x);
}

inline uint32_t _iget_offset24(uint32_t x, uint32_t y, const GraphicsInfo& info) {
    return (y * (info.pitch / 3) + x);
}

inline uint32_t _iget_offset32(uint32_t x, uint32_t y, const GraphicsInfo& info) {
    return (y * (info.pitch / 4) + x);
}

inline void _splm_effective(uint32_t pixoffset, int count) {
    for (int i = 0; i <= (count >> splblocksize) +1 ; ++i) {
        _spl_effective((uint64_t)pixoffset + (i << splblocksize));
    }
}

inline void _iput_line32(uint32_t x, uint32_t y, uint32_t* line, uint32_t length, const GraphicsInfo& target_info) {
    memcpy((void*)_iget_addr32(x, y, target_info), line, length * 4);
    _splm_effective(_iget_offset32(x, y, target_info), length);
}

inline void _iput_line24(uint32_t x, uint32_t y, _co_uint24_t* line, uint32_t length, const GraphicsInfo& target_info) {
    memcpy((void*)_iget_addr24(x, y, target_info), line, length * 3);
    _splm_effective(_iget_offset24(x, y, target_info), length);
}

inline void _imemset_spec24(_co_uint24_t* dest, _co_uint24_t color, uint32_t count) {
    *dest = color;

    if (count > 1) {
        int filled = 1;
        while (filled * 2 <= count) {
            memcpy((char*)dest + filled * 3,
                       (char*)dest,
                       filled * 3);
            filled *= 2;
        }

        if (filled < count) {
            memcpy((char*)dest + filled * 3,
                       (char*)dest,
                       (count - filled) * 3);
        }
    }

}

inline void _imemset_spec32(uint32_t* dest, uint32_t color, uint32_t count) {
    *dest = color;

    if (count > 1) {
        int filled = 1;
        while (filled * 2 <= count) {
            memcpy((char*)dest + filled * 3,
                       (char*)dest,
                       filled * 3);
            filled *= 2;
        }

        if (filled < count) {
            memcpy((char*)dest + filled * 3,
                       (char*)dest,
                       (count - filled) * 3);
        }
    }
}

/
inline void _iputf_line24(uint32_t x, uint32_t y, _co_uint24_t col, uint32_t length, const GraphicsInfo& target_info) {
    _imemset_spec24(_iget_addr24(x, y, target_info), col, length);
    _splm_effective(_iget_offset24(x, y, target_info), length);
}

/
inline void _iputf_line32(uint32_t x, uint32_t y, uint32_t col, uint32_t length, const GraphicsInfo& target_info) {
    _imemset_spec32(_iget_addr32(x, y, target_info), col, length);
    _splm_effective(_iget_offset32(x, y, target_info), length);
}

_co_uint24_t getcol(_co_uint24_t color, const GraphicsInfo& inf) {

    if (inf.rgb) {
        uint8_t tmp = color.byte1;
        color.byte1 = color.byte3;
        color.byte3 = tmp;
    }
    return color;
}

int getcol(int color, const GraphicsInfo& inf) {

    if (inf.rgb) {
        uint8_t tmp = color & 0xFF;
        color = color >> 8;
        color = color | (tmp << 16);
    }
    return color;
}

GraphicsInfo f;
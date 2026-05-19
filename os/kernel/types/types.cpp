#pragma once

#include "scalar.cpp"

#ifndef onlystd
#pragma pack(push, 1)
struct [[gnu::packed]] _co_uint24_t {
    uint8_t byte1;
    uint8_t byte2;
    uint8_t byte3;
};
#pragma pack(pop)

_co_uint24_t uint24(uint32_t value) {
    return _co_uint24_t{(uint8_t)((value >> 16) & 0xFF), (uint8_t)((value >> 8) & 0xFF), (uint8_t)(value & 0xFF)};
}
#include "aligned.cpp"
#include "constant.cpp"
#endif


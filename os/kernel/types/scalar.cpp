#ifndef _INCSCALAR
#define _INCSCALAR

typedef signed char int8_t;
typedef unsigned char uint8_t;

typedef signed short int16_t;
typedef unsigned short uint16_t;

typedef signed int int32_t;
typedef unsigned int uint32_t;

typedef signed long long int64_t;
typedef unsigned long long uint64_t;

typedef int32_t intptr_t;
typedef uint32_t uintptr_t;

#pragma pack(push, 1)
struct _co_uint24_t {
    uint8_t byte1;
    uint8_t byte2;
    uint8_t byte3;
};
#pragma pack(pop)

_co_uint24_t uint24(uint32_t value) {
    return _co_uint24_t{(uint8_t)((value >> 16) & 0xFF), (uint8_t)((value >> 8) & 0xFF), (uint8_t)(value & 0xFF)};
}

#define size_t _size_t
typedef uint64_t _size_t;

typedef size_t uur;
typedef int32_t ssize_t;

#endif
#pragma once

#include "../../utils/utils.cpp"
#include "compute.cpp"
#define splblocksize 8
#define splpixels (1 << splblocksize)

#define splcacheKB 16
#define splcache (splcacheKB*1024)


#define splccblocks (splcache / splpixels)

struct HeatUnit;

struct Splame {
    alignas(16) static __uint128_t map[splamecount];
    alignas(16) static HeatUnit units[splamecount];
    alignas(16) static uint8_t result[splamecount];
    static uint8_t factor;
    static int maxcount;

    static void init();

    static void measure() {
        //_sp_bitcount(map, result);
        //static int counter = 0;
        //if (counter++ == 100) {










//



        //}
    }

    static void prefetch(int index, _co_uint24_t* addr) {
        if (result[index] > factor)
            __builtin_prefetch(addr, 0, 0);
    }
};

alignas(16) __uint128_t Splame::map[splamecount] = {};

struct HeatUnit {
    short index;

    void Effective() const {
        Splame::map[index] |= 1;
    }

    void Passive() const {
        //Splame::map[index] = Splame::map[index] << 1;
    }
};

void Splame::init() {
    for (int i = 0; i < splamecount; i++) {
        units[i].index = i;
        map[i] = 0;
    }
}

alignas(16) HeatUnit Splame::units[splamecount] = {};
alignas(16) uint8_t Splame::result[splamecount] = {};
uint8_t Splame::factor = 0x7F;
int Splame::maxcount = splamecount;
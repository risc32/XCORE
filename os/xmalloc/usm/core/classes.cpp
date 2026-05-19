#pragma once

#include <loctypes>

#define HUGEPOWER 16

namespace xmalloc {
    struct SizeClass {
        void* start;
        size_t free;

        bool present;
        bool ispower;
        bool islarge;
        size_t objsize;
        size_t nobjs;

        size_t size;

        void init() {
            size = objsize * nobjs;
        }
    };
}
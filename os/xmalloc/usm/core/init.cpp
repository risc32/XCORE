#pragma once

#include "../xmalloc.hpp"

#define POOLS 16
#define STARTPOOL 4

#include "classes.cpp"

namespace xmalloc {
#ifdef AUTONOM
#include <utils>
#endif
}

inline void xmalloc::allocator::init() {
    for (auto& class_: classes) {
        class_ = {};
    }

    for (int i = STARTPOOL; i < POOLS; i++) {
        classes[i - STARTPOOL]->present = true;
        classes[i - STARTPOOL]->ispower = true;
        classes[i - STARTPOOL]->objsize = 1 << i;
        classes[i - STARTPOOL]->nobjs = size / (1 << i) / POOLS;
        classes[i - STARTPOOL]->init();
    }

    classes[ALLPOOLS - 1]->islarge = true;
    classes[ALLPOOLS - 1]->nobjs = 1;
    classes[ALLPOOLS - 1]->present = true;

    size_t count = 0;
    for (int i = 0; i < ALLPOOLS; ++i) {
        count += classes[i]->present;
    }

    size_t space = (size / count) & ~127;
    int i = 0;
    for (auto sc: classes) {
        if (!sc->present) continue;
        sc->start = (void*)((char*)start + space * i++);
    }
}
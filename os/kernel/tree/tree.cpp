#pragma once

#include "../cpu/cpu.cpp"
#include "../disk/disk.cpp"
#include "../xcfs/storage.cpp"
#include "../xcfs/bitmap.cpp"
#include "../stream/stream.cpp"
#include "fragment.cpp"

struct path {
    struct changeable {
        uint64_t* data;
        int index;

        operator bool () {
            return get_bit((char*)&data, index);
        }

        changeable& operator=(bool k) {
            set_bit((char*)&data, index, k);
            return *this;
        }
    };
    uint64_t data;
    int size = 0;

    bool operator[](int index) {
        return changeable{.data = &data, .index = index};
    }
    path operator+(bool k) const {
        path p = *this;
        set_bit((char*)&p.data, p.size++, k);
        return p;
    }
    uint64_t* begin() {
        return &data;
    }
    uint64_t* end() {
        return &data+size;
    }
};














































































































































































































































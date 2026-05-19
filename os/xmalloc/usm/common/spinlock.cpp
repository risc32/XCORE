#pragma once

#include "../standard/standard.hpp"

namespace xmalloc {
    class SpinLock {
        volatile bool locked;

        void lock() {
            while (locked) {
                asm volatile ("pause");
            }
            //__atomic_add_fetch(&locked, true, );
        }
        void unlock() {
            locked = false;
        }
    };
}

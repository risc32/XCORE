#pragma once

#include "../standard/standard.hpp"

namespace xmalloc {
    class SpinLock {
        volatile bool locked;

        void lock() {
            while (locked) {
                asm volatile ("pause");
            }
                    }
        void unlock() {
            locked = false;
        }
    };
}

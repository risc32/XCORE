#pragma once

#include "atomic.cpp"
#include "mutex.cpp"
#include "spinlock.cpp"
#include "semaphore.cpp"

#include <utils>
typedef int64_t __guard;

extern "C" {

    union guard_t {
        struct {
            uint8_t initialized;
            uint8_t locked;
            uint16_t _pad;
            uint32_t _pad2;
        };
        int64_t value;
    };

    int __cxa_guard_acquire(__guard* g) {
        guard_t* guard = reinterpret_cast<guard_t*>(g);

        if (guard->initialized) {
            return 0;
        }

        uint8_t expected = 0;
        if (atomic_compare_exchange(&guard->locked, expected, uint8_t(1))) {

            return 1;
        }

        while (true) {
            asm volatile("pause");

            if (guard->initialized) {
                return 0;
            }

            expected = 0;
            if (atomic_compare_exchange(&guard->locked, expected, uint8_t(1))) {
                return 1;
            }
        }
    }

    void __cxa_guard_release(__guard* g) {
        guard_t* guard = reinterpret_cast<guard_t*>(g);

        atomic_store(&guard->initialized, (uint8_t)1);

        atomic_store(&guard->locked, (uint8_t)0);
    }

    void __cxa_guard_abort(__guard* g) {

        guard_t* guard = reinterpret_cast<guard_t*>(g);
        atomic_store(&guard->locked, (uint8_t)0);
    }

}

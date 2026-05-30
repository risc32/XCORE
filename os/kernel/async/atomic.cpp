#pragma once

#include "../types/scalar.cpp"

static inline void memory_barrier() {
    asm volatile("mfence" ::: "memory");
}

static inline void load_barrier() {
    asm volatile("lfence" ::: "memory");
}

static inline void store_barrier() {
    asm volatile("sfence" ::: "memory");
}

static inline void compiler_barrier() {
    asm volatile("" ::: "memory");
}

template<typename T>
static inline T atomic_load(const volatile T* ptr) {
    T val;
    asm volatile("mfence" ::: "memory");
    val = *ptr;
    asm volatile("mfence" ::: "memory");
    return val;
}

template<typename T>
static inline void atomic_store(volatile T* ptr, T val) {
    asm volatile("mfence" ::: "memory");
    *ptr = val;
    asm volatile("mfence" ::: "memory");
}

template<typename T>
static inline T atomic_fetch_add(volatile T* ptr, T val) {
    return __sync_fetch_and_add(ptr, val);
}

template<typename T>
static inline T atomic_fetch_sub(volatile T* ptr, T val) {
    return __sync_fetch_and_sub(ptr, val);
}

template<typename T>
static inline T atomic_fetch_and(volatile T* ptr, T val) {
    return __sync_fetch_and_and(ptr, val);
}

template<typename T>
static inline T atomic_fetch_or(volatile T* ptr, T val) {
    return __sync_fetch_and_or(ptr, val);
}

template<typename T>
static inline T atomic_fetch_xor(volatile T* ptr, T val) {
    return __sync_fetch_and_xor(ptr, val);
}

template<typename T>
static inline T atomic_exchange(volatile T* ptr, T val) {
    return __sync_lock_test_and_set(ptr, val);
}

template<typename T>
static inline bool atomic_compare_exchange(volatile T* ptr, T expected, T desired) {
    return __sync_bool_compare_and_swap(ptr, expected, desired);
}

template<typename T>
static inline T atomic_compare_exchange_val(volatile T* ptr, T expected, T desired) {
    return __sync_val_compare_and_swap(ptr, expected, desired);
}

struct spinlock {
    volatile uint8_t _lock = 0;

    void acquire() {
        while (!__sync_bool_compare_and_swap(&_lock, 0, 1)) {
            asm volatile("pause");
        }
        asm volatile("" ::: "memory");
    }

    void release() {
        asm volatile("" ::: "memory");
        __sync_lock_release(&_lock);
    }

    bool try_acquire() {
        return __sync_bool_compare_and_swap(&_lock, 0, 1);
    }
};

struct scoped_lock {
    spinlock& _sl;

    explicit scoped_lock(spinlock& sl) : _sl(sl) {
        _sl.acquire();
    }

    ~scoped_lock() {
        _sl.release();
    }
};

template<typename T>
struct atomic {
    volatile T _val;

    atomic() : _val(0) {}
    atomic(T v) : _val(v) {}
    atomic(const atomic& other) : _val(other.load()) {}
    atomic& operator=(const atomic& other) {
        if (this != &other) {
            store(other.load());
        }
        return *this;
    }
    atomic(atomic&& other) noexcept : _val(other.load()) {}
    atomic& operator=(atomic&& other) noexcept {
        if (this != &other) {
            store(other.load());
        }
        return *this;
    }


    T load() const {
        return atomic_load(&_val);
    }

    void store(T v) {
        atomic_store(&_val, v);
    }

    T exchange(T v) {
        return atomic_exchange(&_val, v);
    }

    bool compare_exchange(T expected, T desired) {
        return atomic_compare_exchange(&_val, expected, desired);
    }

    T fetch_add(T v) { return atomic_fetch_add(&_val, v); }
    T fetch_sub(T v) { return atomic_fetch_sub(&_val, v); }
    T fetch_and(T v) { return atomic_fetch_and(&_val, v); }
    T fetch_or (T v) { return atomic_fetch_or (&_val, v); }
    T fetch_xor(T v) { return atomic_fetch_xor(&_val, v); }

    T operator++()    { return fetch_add(1) + 1; }
    T operator++(int) { return fetch_add(1); }
    T operator--()    { return fetch_sub(1) - 1; }
    T operator--(int) { return fetch_sub(1); }

    T operator+=(T v) { return fetch_add(v) + v; }
    T operator-=(T v) { return fetch_sub(v) - v; }
    T operator&=(T v) { return fetch_and(v) & v; }
    T operator|=(T v) { return fetch_or (v) | v; }
    T operator^=(T v) { return fetch_xor(v) ^ v; }

    operator T() const { return load(); }
    T operator=(T v) { store(v); return v; }
};

using atomic_bool   = atomic<bool>;
using atomic_int8   = atomic<int8_t>;
using atomic_uint8  = atomic<uint8_t>;
using atomic_int16  = atomic<int16_t>;
using atomic_uint16 = atomic<uint16_t>;
using atomic_int32  = atomic<int32_t>;
using atomic_uint32 = atomic<uint32_t>;
using atomic_int64  = atomic<int64_t>;
using atomic_uint64 = atomic<uint64_t>;

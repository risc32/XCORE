#pragma once

#include "atomic.cpp"
#include "../types/scalar.cpp"

struct mutex {
    atomic<uint8_t> _lock{0};

    void lock() {
        while (true) {
            if (_lock.compare_exchange(0, 1)) {
                return;
            }

            //asm volatile("pause");

            while (_lock.load() != 0) {
                //asm volatile("pause");
            }
        }
    }

    void unlock() {
        memory_barrier();
        _lock.store(0);
    }

    bool try_lock() {
        return _lock.compare_exchange(0, 1);
    }
};

struct timed_mutex {
    atomic<uint64_t> _lock{0};

    void lock() {
        while (!_lock.compare_exchange(0, 1)) {
            asm volatile("pause");
        }
    }

    bool try_lock() {
        return _lock.compare_exchange(0, 1);
    }

    bool try_lock_for(uint64_t microseconds) {
        uint64_t start = get_timestamp_us();
        while (!_lock.compare_exchange(0, 1)) {
            if (get_timestamp_us() - start >= microseconds) {
                return false;
            }
            asm volatile("pause");
        }
        return true;
    }

    void unlock() {
        _lock.store(0);
    }

private:
    uint64_t get_timestamp_us() {
        uint64_t ts;
        asm volatile("rdtsc" : "=A"(ts));
        return ts / 3000;
    }
};

template<typename Mutex>
struct lock_guard {
    Mutex& _mutex;

    explicit lock_guard(Mutex& mtx) : _mutex(mtx) {
        _mutex.lock();
    }

    ~lock_guard() {
        _mutex.unlock();
    }

    lock_guard(const lock_guard&) = delete;
    lock_guard& operator=(const lock_guard&) = delete;
};

template<typename Mutex>
struct unique_lock {
    Mutex* _mutex = nullptr;
    bool _owns = false;

    explicit unique_lock(Mutex& mtx) : _mutex(&mtx) {
        _mutex->lock();
        _owns = true;
    }

    unique_lock(Mutex& mtx, bool defer_lock) : _mutex(&mtx) {
        _owns = !defer_lock;
        if (!defer_lock) {
            _mutex->lock();
        }
    }

    ~unique_lock() {
        if (_owns) {
            _mutex->unlock();
        }
    }

    void lock() {
        if (_mutex && !_owns) {
            _mutex->lock();
            _owns = true;
        }
    }

    void unlock() {
        if (_mutex && _owns) {
            _mutex->unlock();
            _owns = false;
        }
    }

    bool try_lock() {
        if (_mutex && !_owns) {
            _owns = _mutex->try_lock();
            return _owns;
        }
        return false;
    }

    bool owns_lock() const { return _owns; }

    unique_lock(const unique_lock&) = delete;
    unique_lock& operator=(const unique_lock&) = delete;

    unique_lock(unique_lock&& other) noexcept
        : _mutex(other._mutex), _owns(other._owns) {
        other._mutex = nullptr;
        other._owns = false;
    }

    unique_lock& operator=(unique_lock&& other) noexcept {
        if (this != &other) {
            if (_owns) _mutex->unlock();
            _mutex = other._mutex;
            _owns = other._owns;
            other._mutex = nullptr;
            other._owns = false;
        }
        return *this;
    }
};
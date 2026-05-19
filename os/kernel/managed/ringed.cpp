#pragma once

#include "managed.cpp"

template<typename T>
struct Ringed : managed<T> {
private:
    uint64_t _current = 0;
public:

    T& next() {
        return managed<T>::operator[](++_current % this->size());
    }

    T& current() {
        return managed<T>::operator[](_current % this->size());
    }

    void reset() {
        _current = 0;
    }
};
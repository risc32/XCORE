#pragma once

#include "../memory/memory.cpp"
#include "../cpu/cpu.cpp"
//#include "initlist.cpp"

#define CAPACITY 32

template<typename T>
T move(T&& obj) {
    return obj;
}

template<typename T>
class managed {
protected:
    T *_data;
    uint64_t _size = 0;
    uint64_t _capacity = CAPACITY;
    bool nullend = false;

    void grow_if_needed() {
        if (_size >= _capacity) {
            while (_size >= _capacity) _capacity = max(CAPACITY, _capacity * 2);
            *this = managed<T>(*this);
        }
    }

public:
    managed() : _size(0), _capacity(CAPACITY), nullend(false) {
        _data = (T *)(calloc(_capacity, sizeof(T)));
    }

    explicit managed(uint64_t capacity) : nullend(false) {
        _capacity = capacity > 0 ? capacity : CAPACITY;
        _data = (T *)(calloc(_capacity, sizeof(T)));
        _size = 0;
    }

    managed(const managed<T>& other) : nullend(other.nullend) {
        _capacity = other._capacity;
        _size = other._size;
        _data = (T*)calloc(_capacity, sizeof(T));
        if (!_capacity) _capacity = CAPACITY;
        if (!_data) panic("Out of memory");

        for (int i = 0; i < _size; ++i) {
            _data[i] = other._data[i];
        }

        // Добавляем нуль-терминатор если нужно
        if (nullend && _size < _capacity) {
            _data[_size] = T();
        }
    }

    managed(uint64_t count, T elem) : _size(0), _capacity(CAPACITY), nullend(false) {
        _data = (T *)(calloc(_capacity, sizeof(T)));
        for (int i = 0; i < count; ++i) {
            push_back(elem);
        }
    }

    ~managed() {
        for (int i = 0; i < _size; ++i) {
            _data[i].~T();
        }
        free(_data);
    }

    managed<T>& operator=(const managed<T>& other) {
        if (this != &other) {
            T* new_data = (T*)calloc(other._capacity, sizeof(T));
            if (!other._capacity) panic("Capacity is null");

            if (!new_data) panic("Out of memory");

            for (int i = 0; i < other._size; ++i) {
                new_data[i] = other._data[i];
            }

            free(_data);
            _data = new_data;
            _size = other._size;
            _capacity = other._capacity;
            nullend = other.nullend;

            // Добавляем нуль-терминатор если нужно
            if (nullend && _size < _capacity) {
                _data[_size] = T();
            }
        }
        return *this;
    }

    void push_back(const T& value) {
        insert(_size, value);
    }

    void push_back(T&& value) {
        insert(_size, move(value));
    }

    void pop_back() {
        remove(_size - 1);
    }

    void insert(int index, const T& value) {
        if (index < 0 || index > _size) panic("Index out of range");

        grow_if_needed();

        for (int i = _size; i > index; i--) {
            _data[i] = _data[i - 1];
        }
        _data[index] = value;
        _size++;

        // Обновляем нуль-терминатор если нужно
        if (nullend && _size < _capacity) {
            _data[_size] = T();
        }
    }

    void remove(int index) {
        if (index < 0 || index >= _size) panic("Index out of range");

        for (int i = index; i < _size - 1; i++) {
            _data[i] = _data[i + 1];
        }
        _size--;

        // Обновляем нуль-терминатор если нужно
        if (nullend && _size < _capacity) {
            _data[_size] = T();
        }
    }

    T &operator[](int index) {
        if (index < 0 || index >= _size) panic("Index out of range");
        return _data[index];
    }

    const T &operator[](int index) const {
        if (index < 0 || index >= _size) panic("Index out of range");
        return _data[index];
    }

    managed<T> operator+(const T& value) const {
        managed res(*this);
        res.push_back(value);
        return res;
    }

    managed<T> operator+(const managed<T>& other) const {
        managed<T> res = *this;
        res += other;
        return res;
    }

    managed<T>& operator+=(const T& value) {
        push_back(value);
        return *this;
    }

    managed<T>& operator+=(const T&& value) {
        push_back(move(value));
        return *this;
    }

    managed<T>& operator+=(const managed<T>& other) {
        int old_size = _size;
        _size += other._size;
        grow_if_needed();

        for (int i = 0; i < other._size; ++i) {
            _data[old_size + i] = other._data[i];
        }

        // Обновляем нуль-терминатор если нужно
        if (nullend && _size < _capacity) {
            _data[_size] = T();
        }
        return *this;
    }

    bool operator==(const managed<T>& other) const {
        if (_size != other._size) return false;

        for (int i = 0; i < _size; ++i) {
            if (_data[i] != other._data[i]) return false;
        }
        return true;
    }

    bool operator!=(const managed<T>& other) const {
        return !(*this == other);
    }

    T *data() {
        return _data;
    }

    const T *data() const {
        return _data;
    }

    uint64_t size() const {
        return _size;
    }

    uint64_t capacity() const {
        return _capacity;
    }

    bool empty() const {
        return _size == 0;
    }

    void clear() {
        _size = 0;
        if (nullend && _capacity > 0) {
            _data[0] = T();
        }
    }

    void reserve(int new_capacity) {
        if (new_capacity > _capacity) {
            _capacity = new_capacity;
            _data = (T *) realloc(_data, _capacity * sizeof(T));

            // Добавляем нуль-терминатор если нужно
            if (nullend && _size < _capacity) {
                _data[_size] = T();
            }
        }
    }

    void setsize(int newsize) {
        if (newsize > _capacity) {
            reserve(newsize);
        }
        _size = newsize;

        // Добавляем нуль-терминатор если нужно
        if (nullend && _size < _capacity) {
            _data[_size] = T();
        }
    }

    void shrink_to_fit() {
        if (_size < _capacity) {
            _capacity = _size > 0 ? _size : 1;
            _data = (T *) realloc(_data, _capacity * sizeof(T));

            // Добавляем нуль-терминатор если нужно
            if (nullend) {
                // Для nullend всегда должен быть дополнительный байт для терминатора
                _capacity = _size + 1;
                _data = (T *) realloc(_data, _capacity * sizeof(T));
                _data[_size] = T();
            }
        }
    }

    template<size_t N>
    explicit managed(const T (&array)[N]) : _size(0), nullend(false) {
        _capacity = N > 0 ? N : CAPACITY;
        _data = (T *)calloc(_capacity, sizeof(T));
        for (size_t i = 0; i < N; i++) {
            push_back(array[i]);
        }
    }

    template<size_t N>
    managed<T>& operator+=(const T (&array)[N]) {
        for (size_t i = 0; i < N; i++) {
            push_back(array[i]);
        }
        return *this;
    }

    template<size_t N>
    managed<T> operator+(const T (&array)[N]) const {
        managed<T> res(*this);
        for (size_t i = 0; i < N; i++) {
            res.push_back(array[i]);
        }
        return res;
    }

    T* begin() {
        return _data;
    }

    const T* begin() const {
        return _data;
    }

    T* end() {
        return _data + _size;
    }

    const T* end() const {
        return _data + _size;
    }
};

#include "string.cpp"
#pragma once

#include "../memory/memory.cpp"

struct StackPool {
    size_t objsize;
    size_t capacity;
    size_t top;
    size_t *map;
    char *memory;
    bool present;

    StackPool(size_t os, uint64_t _capacity) : objsize(os) {
        present = true;
        top = capacity = _capacity;
        map = static_cast<size_t *>(promise(sizeof(*map) * capacity));
        memory = static_cast<char *>(promise(objsize * capacity));

        for (size_t i = 0; i < capacity; i++) {
            map[i] = capacity - 1 - i;
        }
    }

    void *alloc() {
        if (top == 0) return nullptr;
        return memory + map[--top] * objsize;
    }

    void free(void *ptr) {
        if (ptr == nullptr) return;
        const size_t index = reinterpret_cast<size_t>(ptr) - reinterpret_cast<size_t>(memory);
        if (top < capacity) map[top++] = index / objsize;
    }
};

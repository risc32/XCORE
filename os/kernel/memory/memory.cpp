#pragma once

#define HEAP_SIZE (1024 * 1024 * 16)
#define MAXB_SIZE 512
#define MINB_SIZE 32
#define ALIGN 32

#include "../types/scalar.cpp"

void memzero(void *, size_t);

struct memory_block {
    int mbid;
    bool used;
    memory_block *next;
    memory_block *back;
    size_t size;
};

struct memory {
    static char heap[HEAP_SIZE];
    static memory_block *first;
    static const memory_block * const last;

    static int mbids;

    static void init() {  ///for stacktrace
        first = (memory_block*)heap;
        first->size = HEAP_SIZE - sizeof(memory_block);
        first->used = false;
        first->next = nullptr;
    }

    static void reset() {
        first = (memory_block*)heap;
        first->size = (1024 * 1024 * 16) - sizeof(memory_block);
        first->used = false;
        first->next = nullptr;
        mbids = 0;

        // Очищаем всю кучу
        //memzero(heap, sizeof(heap));

        // Переинициализируем первый блок
        first->size = (1024 * 1024 * 16) - sizeof(memory_block);
        first->used = false;
        first->next = nullptr;
    }

    static size_t align_size(size_t size) {
        const size_t MIN_BLOCK_SIZE = sizeof(memory_block) + ALIGN;
        if (size < MIN_BLOCK_SIZE) {
            size = MIN_BLOCK_SIZE;
        }
        size = (size + (ALIGN - 1)) & ~(ALIGN - 1);
        return size;
    }
};

char memory::heap[HEAP_SIZE] = {};
memory_block *memory::first = (memory_block *) memory::heap;
const memory_block * const memory::last = (memory_block *) memory::heap;

int memory::mbids = 0;

#include "basic/memchr.cpp"
#include "basic/memcmp.cpp"
#include "basic/memcpy.cpp"
#include "basic/memmove.cpp"
#include "basic/memset.cpp"
#include "allocate.cpp"
#include "free.cpp"
#include "realloc.cpp"


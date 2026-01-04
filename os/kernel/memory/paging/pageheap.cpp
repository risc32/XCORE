#pragma once

#include "../../utils/utils.cpp"
#include "../memory.cpp"

#define PHSIZE (1024*1024*4)
#define PAGESIZE 4096
#define PAGES_PER_BYTE 8

struct PageHeap {
    alignas(4096) static unsigned char heap[PHSIZE];
    static unsigned char usedmap[PHSIZE / PAGESIZE / PAGES_PER_BYTE];

    static void init() {

        memset(usedmap, 0, sizeof(usedmap));
    }

    static void* alloc() {
        for (size_t i = 0; i < sizeof(usedmap); i++) {
            if (usedmap[i] == 0xFF) {
                continue;
            }

            for (int j = 0; j < PAGES_PER_BYTE; j++) {
                if ((usedmap[i] >> j) & 1) {
                    continue;
                }


                usedmap[i] |= (1 << j);
                size_t page_index = i * PAGES_PER_BYTE + j;
                return &heap[page_index * PAGESIZE];
            }
        }
        return nullptr;
    }

    static void free(void* ptr) {
        if (!ptr) return;


        uintptr_t heap_start = (uintptr_t)&heap[0];
        uintptr_t heap_end = heap_start + PHSIZE;
        uintptr_t ptr_addr = (uintptr_t)ptr;

        if (ptr_addr < heap_start || ptr_addr >= heap_end) {

            return;
        }


        size_t page_index = (ptr_addr - heap_start) / PAGESIZE;
        size_t map_index = page_index / PAGES_PER_BYTE;
        size_t bit_index = page_index % PAGES_PER_BYTE;


        usedmap[map_index] &= ~(1 << bit_index);


        memset(ptr, 0, PAGESIZE);
    }
};

alignas(4096) unsigned char PageHeap::heap[PHSIZE] = {};
unsigned char PageHeap::usedmap[PHSIZE/PAGESIZE/PAGES_PER_BYTE] = {};
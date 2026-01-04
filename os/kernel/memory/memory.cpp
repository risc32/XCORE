#pragma once

#include "../types/scalar.cpp"
#include "../debug/debug.cpp"

struct MemoryInfoE801 {
    uint16_t memory_1mb_16mb;
    uint16_t memory_above_16mb;

    uint32_t total() {


        uint32_t total_kb = memory_1mb_16mb;


        total_kb += memory_above_16mb * 64;


        total_kb += 640;

        return total_kb;
    }
};
MemoryInfoE801 detect_memory() {
    MemoryInfoE801 info = {};

    asm volatile (
            "mov $0xE801, %%ax\n\t"
            "int $0x15\n\t"
            "mov %%ax, %0\n\t"
            "mov %%bx, %1\n\t"
            : "=r" (info.memory_1mb_16mb), "=r" (info.memory_above_16mb)
            :
            : "ax", "bx", "cx", "dx", "memory"
            );

    return info;
}

#define HEAP_SIZE (80*1024*1024)
#define MAXB_SIZE 512
#define MINB_SIZE 32
#define ALIGN 64


void memzero(void *, size_t);

struct alignas(64) memory_block {
    int mbid;
    bool used;
    memory_block *next;
    memory_block *back;
    size_t size;
};

struct memory {

    alignas(64) static char heap[HEAP_SIZE];
    static memory_block *first;
    static const memory_block * const last;

    static int mbids;

    static void init() {  ///for stacktrace

        first = (memory_block*)heap;
        first->size = HEAP_SIZE - sizeof(memory_block);
        first->used = false;
        first->next = nullptr;

        s0::put("void memory::init() KERNEL 0x20000 .text\n");
    }

    static void reset() {
        first = (memory_block*)heap;
        first->size = (1024 * 1024 * 16) - sizeof(memory_block);
        first->used = false;
        first->next = nullptr;
        mbids = 0;


        //memzero(heap, sizeof(heap));


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

uint64_t _map_phys(uint64_t phys_addr, uint64_t size) {

    uint64_t aligned_phys = phys_addr & ~0xFFFULL;
    uint64_t offset = phys_addr - aligned_phys;


    uint64_t aligned_size = (size + offset + 0xFFF) & ~0xFFFULL;


    for(uint64_t i = 0; i < aligned_size; i += 0x1000) {
        uint64_t page_phys = aligned_phys + i;
        uint64_t page_virt = 0 + i;



    }


    //return (void*)(0 + offset);
}

alignas(64) char memory::heap[HEAP_SIZE] = {};
memory_block *memory::first = (memory_block *) memory::heap;
const memory_block * const memory::last = (memory_block *) memory::heap;

int memory::mbids = 0;

#include "basic/memchr.cpp"
#include "basic/memcmp.cpp"
#include "basic/memcpy.cpp"
#include "basic/memmove.cpp"
#include "basic/memset.cpp"

#ifndef stage2
#include "allocate.cpp"
#include "free.cpp"
#include "realloc.cpp"
#endif

#include "paging/paging.cpp"

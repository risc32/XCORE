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

#define HEAP_SIZE (120*1024*1024)
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

    void* getdata() {
        return this+1;
    }
};



#ifndef stage2
#include "allocv2.cpp"

struct memory {
    alignas(64) static char heap[HEAP_SIZE];
    static AllocV2 kheap;

    static void init();

    // Обертки для удобства
    static void* kmalloc(size_t size);
    static void kfree(void* ptr);
    static void *krealloc(void* ptr, size_t size);
};

extern "C" void *allocate(size_t size) {return memory::kmalloc(size); }
extern void* malloc(size_t size) { return memory::kmalloc(size); }
extern void free(void* ptr) { memory::kfree(ptr); }
extern "C" void* realloc(void* ptr, size_t size) { return memory::krealloc(ptr, size); }
void *calloc(size_t num, size_t size) {
    size_t total_size = num * size;
    void *ptr = allocate(total_size);

    if (ptr) {
        memzero(ptr, total_size);
    }

    return ptr;
}

// Операторы для C++ объектов
void* operator new(size_t size) { return malloc(size); }
void* operator new[](size_t size) { return malloc(size); }
void operator delete(void* p) noexcept { free(p); }
void operator delete[](void* p) noexcept { free(p); }
void operator delete(void* p, size_t) noexcept { free(p); }

void memory::init() {
    kheap.init(heap, HEAP_SIZE);
    s0::put("SafeAllocator initialized on kernel heap (80MB)\n");
}

void* memory::kmalloc(_size_t size) {
    return kheap.malloc(size);
}

void memory::kfree(void* ptr) {
    kheap.free(ptr);
}

void *memory::krealloc(void* ptr, size_t size) {
    return kheap.realloc(ptr, size);
}

// Инициализация статических членов
alignas(64) char memory::heap[HEAP_SIZE] = {};
AllocV2 memory::kheap = {};
#endif


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

#include "basic/memchr.cpp"
#include "basic/memcmp.cpp"
#include "basic/memcpy.cpp"
#include "basic/memmove.cpp"
#include "basic/memset.cpp"

#ifndef stage2
//#include "allocate.cpp"
//#include "free.cpp"
//#include "realloc.cpp"
#endif

#include "paging/paging.cpp"

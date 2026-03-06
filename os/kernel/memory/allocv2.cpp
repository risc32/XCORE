#pragma once
#include "../types/scalar.cpp"

namespace HeapConfig {
    constexpr uint32_t MAGIC_HEADER = 0x414C4C4F;
    constexpr uint32_t CANARY       = 0xDEADC0DE;
    constexpr uint8_t  POISON_FREE  = 0xA5;
    constexpr size_t   ALIGNMENT    = 64;
}

void _panic(const char* func, const char *message);
#define panic(msg) _panic(__PRETTY_FUNCTION__, msg)

struct AllocV2 {

    struct alignas(HeapConfig::ALIGNMENT) MemoryBlock {
        uint32_t magic;
        size_t size;
        MemoryBlock* next;
        uint32_t canary;
        bool is_free;


        void* data() { return (void*)(this + 1); }
    };

private:
    MemoryBlock* head = nullptr;
    uint64_t heap_start_addr = 0;
    uint64_t heap_end_addr = 0;

    size_t align_up(size_t size) {
        return (size + HeapConfig::ALIGNMENT - 1) & ~(HeapConfig::ALIGNMENT - 1);
    }

    void validate_block(MemoryBlock* block) {
        if (!block) return;
        if (block->magic != HeapConfig::MAGIC_HEADER)
            panic("Header magic corruption (possible buffer overflow from previous block)");
        if (block->canary != HeapConfig::CANARY)
            panic("Buffer underflow (canary corrupted)");
    }

public:
    void init(void* start, size_t total_size) {
        uintptr_t raw_addr = (uintptr_t)start;
        uintptr_t aligned_addr = align_up(raw_addr);


        if (total_size <= (aligned_addr - raw_addr) + sizeof(MemoryBlock)) return;
        total_size -= (aligned_addr - raw_addr);

        head = (MemoryBlock*)aligned_addr;
        head->magic = HeapConfig::MAGIC_HEADER;
        head->canary = HeapConfig::CANARY;
        head->is_free = true;
        head->next = nullptr;


        head->size = total_size - sizeof(MemoryBlock);

        heap_start_addr = (uint64_t)aligned_addr;
        heap_end_addr = heap_start_addr + total_size;
    }

    void* malloc(size_t size) {
        if (size == 0) return nullptr;
        size = align_up(size);

        MemoryBlock* curr = head;
        while (curr) {
            if (curr->is_free && curr->size >= size) {
                validate_block(curr);



                size_t min_split_size = sizeof(MemoryBlock) + HeapConfig::ALIGNMENT;

                if (curr->size >= size + min_split_size) {


                    uint8_t* next_block_addr = (uint8_t*)curr->data() + size;
                    MemoryBlock* next_block = (MemoryBlock*)next_block_addr;


                    next_block->size = curr->size - size - sizeof(MemoryBlock);
                    next_block->magic = HeapConfig::MAGIC_HEADER;
                    next_block->canary = HeapConfig::CANARY;
                    next_block->is_free = true;
                    next_block->next = curr->next;


                    curr->size = size;
                    curr->next = next_block;
                }

                curr->is_free = false;
                return curr->data();
            }
            curr = curr->next;
        }
        return nullptr;
    }

    void free(void* ptr) {
        if (!ptr) return;


        if ((uintptr_t)ptr < heap_start_addr || (uintptr_t)ptr >= heap_end_addr)
            panic("Pointer outside the heap boundaries");

        MemoryBlock* block = (MemoryBlock*)((uint8_t*)ptr - sizeof(MemoryBlock));
        validate_block(block);

        if (block->is_free) panic("Double free detected");

#ifdef MMDEBUG
        uint8_t* d = (uint8_t*)ptr;
        for (size_t i = 0; i < block->size; i++) d[i] = HeapConfig::POISON_FREE;
#endif
        block->is_free = true;


        MemoryBlock* curr = head;
        while (curr && curr->next) {
            if (curr->is_free && curr->next->is_free) {


                uintptr_t next_addr = (uintptr_t)curr->next;
                uintptr_t curr_end = (uintptr_t)curr->data() + curr->size;
                size_t gap = next_addr - curr_end;


                curr->size += gap + sizeof(MemoryBlock) + curr->next->size;
                curr->next = curr->next->next;


            } else {
                curr = curr->next;
            }
        }
    }

    void* realloc(void* ptr, size_t size) {
        if (!ptr) return malloc(size);
        if (size == 0) { free(ptr); return nullptr; }

        MemoryBlock* block = (MemoryBlock*)((uint8_t*)ptr - sizeof(MemoryBlock));
        validate_block(block);

        size = align_up(size);


        if (block->size >= size) return ptr;


        void* new_ptr = malloc(size);
        if (new_ptr) {

            size_t copy_size = block->size;
            uint8_t* src = (uint8_t*)ptr;
            uint8_t* dst = (uint8_t*)new_ptr;
            for(size_t i = 0; i < copy_size; i++) dst[i] = src[i];

            free(ptr);
        }
        return new_ptr;
    }
};
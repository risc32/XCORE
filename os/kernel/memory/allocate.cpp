#pragma once

#include "memory.cpp"
#include "../stream/cstatic.cpp"

void *allocate(size_t size) {
    if (!size) return nullptr;

    size = memory::align_size(size);
    memory_block *block = memory::first;

    while (block) {
        if (!block->used && block->size >= size) {

            size_t remaining = block->size - size;


            if (remaining >= sizeof(memory_block) + MINB_SIZE) {
                auto *new_block = (memory_block *)(
                        (char *)block + sizeof(memory_block) + size
                );

                new_block->size = remaining - sizeof(memory_block);
                new_block->used = false;
                new_block->next = block->next;
                new_block->back = block;

                if (block->next) {
                    block->next->back = new_block;
                }

                block->size = size;
                block->next = new_block;
            }

            block->used = true;
            block->mbid = memory::mbids++;
            return (void *)((char *)block + sizeof(memory_block));
        }
        block = block->next;
    }
    return nullptr;
}

void *calloc(size_t num, size_t size) {
    size_t total_size = num * size;
    void *ptr = allocate(total_size);

    if (ptr) {
        memzero(ptr, total_size);
    }

    return ptr;
}

void *malloc(size_t size) {
    void *ptr = allocate(size);

    if (ptr) {
        memzero(ptr, size);
    }

    return ptr;
}
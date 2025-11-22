#pragma once

#include "memory.cpp"

void free(void *ptr) {
    if (!ptr) return;


    if (ptr < (void*)memory::heap || ptr >= (void*)(memory::heap + sizeof(memory::heap))) {
        return;
    }

    auto *block = (memory_block *)((char *)ptr - sizeof(memory_block));


    if (block->mbid < 0 || block->mbid >= memory::mbids) {
        return;
    }

    block->used = false;

    if (block->back && !block->back->used) {
        size_t original_size = block->size;

        block->back->size += original_size + sizeof(memory_block);
        block->back->next = block->next;

        if (block->next) {
            block->next->back = block->back;
        }

        block = block->back;
    }

    if (block->next && !block->next->used) {
        block->size += block->next->size + sizeof(memory_block);
        block->next = block->next->next;

        if (block->next) {
            block->next->back = block;
        }
    }
}
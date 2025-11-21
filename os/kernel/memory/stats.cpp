#pragma once

#include "memory.cpp"

struct stats {
    size_t total_blocks;
    size_t used_blocks;
    size_t free_blocks;
    size_t total_memory;
    size_t used_memory;
    size_t free_memory;
    size_t fragmentation;
};

stats get_stats() {  ///for stacktrace

    stats stats = {0};

    memory_block *current = memory::first;
    while (current) {
        stats.total_blocks++;

        if (current->used) {
            stats.used_blocks++;
            stats.used_memory += current->size;
        } else {
            stats.free_blocks++;
            stats.free_memory += current->size;
        }

        stats.total_memory += current->size + sizeof(memory_block);
        current = current->next;
    }

    if (stats.free_blocks > 1) {
        stats.fragmentation = (stats.free_blocks - 1) * 100 / stats.total_blocks;
    }

    return stats;
}
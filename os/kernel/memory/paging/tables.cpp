#pragma once

#include "../../types/types.cpp"
#include "record.cpp"

template<typename T> struct [[gnu::packed]] Table {
    T entries[512];

    T* operator[](size_t index) {
        return &entries[index];
    }

    const T* operator[](size_t index) const {
        return &entries[index];
    }
};

typedef Table<PML4Entry> PML4Table;
typedef Table<PDPTEntry> PDPTTable;
typedef Table<PDEntry> PDTable;
typedef Table<PTEntry> PTTable;

struct [[gnu::packed]] VirtualAddress {
    union {
        uint64_t raw;

        struct {
            uint64_t offset    : 12;
            uint64_t pt  : 9;
            uint64_t pd  : 9;
            uint64_t pdpt : 9;
            uint64_t pml4 : 9;

            uint64_t sign_ext  : 16;
        } KB4;

        struct {
            uint64_t offset : 21;
            uint64_t pd : 9;
            uint64_t pdpt : 9;
            uint64_t pml4 : 9;
            uint64_t : 16;
        } MB2;

        struct {
            uint64_t offset : 30;
            uint64_t pdpt : 9;
            uint64_t pml4 : 9;
            uint64_t : 16;
        } GB1;
    };
};
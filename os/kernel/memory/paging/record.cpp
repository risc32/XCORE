#pragma once

#include "../../types/types.cpp"

#pragma pack(push, 1)


constexpr uintptr_t ADDRESS_MASK_4KB = 0x0000FFFFFFFFF000ULL;
constexpr uintptr_t ADDRESS_MASK_2MB = 0x0000FFFFFFE00000ULL;
constexpr uintptr_t ADDRESS_MASK_1GB = 0x0000FFFFC0000000ULL;


struct PML4Entry {
    uint64_t present    : 1;
    uint64_t writable   : 1;
    uint64_t user       : 1;
    uint64_t pwt        : 1;
    uint64_t pcd        : 1;
    uint64_t accessed   : 1;
    uint64_t ignored1   : 1;
    uint64_t reserved1  : 1;
    uint64_t ignored2   : 4;
    uint64_t address    : 40;
    uint64_t ignored3   : 11;
    uint64_t nx         : 1;

    void* get_pdpt_address() const {
        return (void*)((uintptr_t)address << 12);
    }

    void set_pdpt_address(void* addr) {
        uintptr_t p = (uintptr_t)addr;
        if (p & 0xFFF) return;
        address = (p >> 12) & 0xFFFFFFFFFULL;
    }

    bool is_present() const { return present; }
    void set_present(bool val) { present = val ? 1 : 0; }

    void set_flags(bool rw = true, bool us = false, bool nx = false) {
        writable = rw ? 1 : 0;
        user = us ? 1 : 0;
        this->nx = nx ? 1 : 0;
    }
};


struct PDPTEntry {
    uint64_t present    : 1;
    uint64_t writable   : 1;
    uint64_t user       : 1;
    uint64_t pwt        : 1;
    uint64_t pcd        : 1;
    uint64_t accessed   : 1;
    uint64_t dirty      : 1;
    uint64_t page_size  : 1;
    uint64_t ignored1   : 4;
    uint64_t address    : 40;
    uint64_t ignored2   : 11;
    uint64_t nx         : 1;

    void* get_address() const {
        if (page_size) {

            return (void*)((uintptr_t)address << 12);
        } else {

            return (void*)((uintptr_t)address << 12);
        }
    }

    void set_address(void* addr, bool is_1gb_page = false) {
        uintptr_t p = (uintptr_t)addr;
        page_size = is_1gb_page ? 1 : 0;

        if (is_1gb_page) {
            if (p & 0x3FFFFFFF) return;
            address = (p >> 12) & 0xFFFFFFFFFULL;
        } else {
            if (p & 0xFFF) return;
            address = (p >> 12) & 0xFFFFFFFFFULL;
        }
    }

    bool is_1gb_page() const { return page_size; }
};


struct PDEntry {
    uint64_t present    : 1;
    uint64_t writable   : 1;
    uint64_t user       : 1;
    uint64_t pwt        : 1;
    uint64_t pcd        : 1;
    uint64_t accessed   : 1;
    uint64_t dirty      : 1;
    uint64_t page_size  : 1;
    uint64_t global     : 1;
    uint64_t pat        : 1;
    uint64_t reserved   : 2;
    uint64_t address    : 40;
    uint64_t ignored    : 11;
    uint64_t nx         : 1;

    void* get_address() const {
        if (page_size) {

            return (void*)(((uintptr_t)address << 12) | (pat << 12));
        } else {

            return (void*)((uintptr_t)address << 12);
        }
    }

    void set_address(void* addr, bool is_2mb_page = false, uint8_t pat_value = 0) {
        uintptr_t p = (uintptr_t)addr;
        page_size = is_2mb_page ? 1 : 0;
        pat = pat_value & 0x1;

        if (is_2mb_page) {
            if (p & 0x1FFFFF) return;
            address = (p >> 12) & 0xFFFFFFFFFULL;
        } else {
            if (p & 0xFFF) return;
            address = (p >> 12) & 0xFFFFFFFFFULL;
        }
    }

    bool is_2mb_page() const { return page_size; }
};


struct PTEntry {
    uint64_t present    : 1;
    uint64_t writable   : 1;
    uint64_t user       : 1;
    uint64_t pwt        : 1;
    uint64_t pcd        : 1;
    uint64_t accessed   : 1;
    uint64_t dirty      : 1;
    uint64_t pat        : 1;
    uint64_t global     : 1;
    uint64_t ignored    : 3;
    uint64_t address    : 40;
    uint64_t ignored2   : 11;
    uint64_t nx         : 1;

    void* get_physical_address() const {
        return (void*)((uintptr_t)address << 12);
    }

    void set_physical_address(void* addr) {
        uintptr_t p = (uintptr_t)addr;
        if (p & 0xFFF) return;
        address = (p >> 12) & 0xFFFFFFFFFULL;
    }
};

#pragma pack(pop)
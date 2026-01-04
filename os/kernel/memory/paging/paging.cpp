#pragma once

#include "../../types/types.cpp"
#include "../../debug/debug.cpp"
#include "../../CPU/cpu.cpp"

#include "tables.cpp"
#include "pageheap.cpp"

#define PML4 0xA000

static inline uint64_t get_pml4_idx(uint64_t addr) { return (addr >> 39) & 0x1FF; }
static inline uint64_t get_pdpt_idx(uint64_t addr) { return (addr >> 30) & 0x1FF; }
static inline uint64_t get_pd_idx(uint64_t addr) { return (addr >> 21) & 0x1FF; }
static inline void invlpg(uint64_t addr) { asm volatile("invlpg (%0)" : : "r"(addr) : "memory"); }

struct record {
    uint64_t virtual_addr;
    uint64_t physical_addr;
    bool writable;
    bool user;
    bool execute;
    bool cache;
    bool global;
    bool create;
};

struct __attribute__((packed)) _mmrec {

};

static inline uint64_t virt_to_phys(void* virt) {
    return (uint64_t)virt;
}

static inline void* phys_to_virt(uint64_t phys) {
    return (void*)phys;
}

struct paging {
    static PML4Table* pml4;

    static bool mmap(
            uint64_t virtual_addr,
            uint64_t physical_addr,
            bool writable = true,
            bool user = false,
            bool execute = true,
            bool cache = false,
            bool global = false,
            bool create = true
    ) {

        if((virtual_addr & 0x1FFFFF) || (physical_addr & 0x1FFFFF)) {
            return false;
        }

        VirtualAddress va{virtual_addr};


        PML4Entry& pml4ent = pml4->entries[va.MB2.pml4];

        if (!pml4ent.present) {

            void* pdpt_virt = PageHeap::alloc();
            if (!pdpt_virt) {
                return false;
            }


            memset(pdpt_virt, 0, 4096);


            uint64_t pdpt_phys = virt_to_phys(pdpt_virt);

            pml4ent.present = 1;
            pml4ent.writable = 1;
            pml4ent.user = 0;
            pml4ent.nx = 0;
            pml4ent.pcd = !cache;
            pml4ent.address = pdpt_phys >> 12;
        }


        uint64_t pdpt_phys = pml4ent.address << 12;
        void* pdpt_virt = phys_to_virt(pdpt_phys);
        PDPTTable* pdpt = (PDPTTable*)pdpt_virt;
        PDPTEntry& pdptent = pdpt->entries[va.MB2.pdpt];

        if (!pdptent.present) {

            void* pd_virt = PageHeap::alloc();
            if (!pd_virt) {
                return false;
            }

            memset(pd_virt, 0, 4096);
            uint64_t pd_phys = virt_to_phys(pd_virt);

            pdptent.present = 1;
            pdptent.writable = 1;
            pdptent.user = 0;
            pdptent.nx = 0;
            pdptent.pcd = !cache;
            pdptent.page_size = 0;
            pdptent.address = pd_phys >> 12;
        }


        uint64_t pd_phys = pdptent.address << 12;
        void* pd_virt = phys_to_virt(pd_phys);
        PDTable* pd = (PDTable*)pd_virt;
        PDEntry& pdent = pd->entries[va.MB2.pd];

        if (!pdent.present) {

            pdent.present = 1;
            pdent.writable = writable ? 1 : 0;
            pdent.user = user ? 1 : 0;
            pdent.nx = execute ? 0 : 1;
            pdent.pcd = !cache;
            pdent.page_size = 1;
            pdent.global = global ? 1 : 0;
            pdent.address = physical_addr >> 12;


            invlpg(virtual_addr);


            //test_access(virtual_addr);

            return true;
        } else {
            return false;
        }
    }


    static bool identity_map(uint64_t addr) {
        return mmap(addr, addr);
    }


    static bool map_range_identity(uint64_t addr, uint64_t size) {
        map_range_custom(addr, addr, size);
        return true;
    }


    static bool map_range_custom(uint64_t virt_start, uint64_t phys_start, uint64_t size) {
        for(uint64_t i = 0; i < size; i += 0x200000) {
            if(!mmap(virt_start + i, phys_start + i)) return false;
        }
        return true;
    }


    static void gmap(int offset, int giga) {
        for (uint64_t i = 0; i < giga*512; i++) {
            paging::identity_map(offset + i * 0x200000);
        }
    }
};

PML4Table* paging::pml4 = (PML4Table*)PML4;
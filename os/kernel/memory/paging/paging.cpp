#pragma once

#include "../../types/types.cpp"
#include "../../debug/debug.cpp"
#include "../../CPU/cpu.cpp"
#include "tables.cpp"
#include "pageheap.cpp"

#define PML4_DEFAULT 0xA000

static inline uint64_t get_pml4_idx(uint64_t addr) { return (addr >> 39) & 0x1FF; }
static inline uint64_t get_pdpt_idx(uint64_t addr) { return (addr >> 30) & 0x1FF; }
static inline uint64_t get_pd_idx(uint64_t addr) { return (addr >> 21) & 0x1FF; }
static inline void invlpg(uint64_t addr) { asm volatile("invlpg (%0)" : : "r"(addr) : "memory"); }

struct PageMapping {
    uint64_t virtual_addr;
    uint64_t physical_addr;
    bool writable;
    bool user;
    bool execute;
    bool cache;
    bool global;
    bool create;
};

struct MappingFlags {
    bool writable = true;
    bool user = true;
    bool execute = true;
    bool cache = false;
    bool global = false;
    bool force = true;
    bool create = true;

    MappingFlags() = default;
    MappingFlags(bool _writable, bool _user, bool _execute, bool _cache, bool _global, bool _force, bool _create)
        : writable(_writable), user(_user), execute(_execute), cache(_cache), global(_global), force(_force), create(_create) {}
};

struct PagingManager {
    PML4Table* m_pml4;
    uint64_t m_pml4_phys;
    bool m_owns_pml4;

    static PagingManager* s_kernel_manager;

    bool validate_alignment(uint64_t virt, uint64_t phys) {
        return !((virt & 0x1FFFFF) || (phys & 0x1FFFFF));
    }

    uint64_t virt_to_phys(void* virt) {
        return (uint64_t)virt;
    }

    void* phys_to_virt(uint64_t phys) {
        return (void*)phys;
    }

    PagingManager() : m_pml4(nullptr), m_pml4_phys(0), m_owns_pml4(false) {}

    explicit PagingManager(PML4Table* pml4, uint64_t pml4_phys = 0)
        : m_pml4(pml4), m_pml4_phys(pml4_phys), m_owns_pml4(false) {}

    ~PagingManager() {
        if (m_owns_pml4 && m_pml4) {
                    }
    }

    bool init_new() {
        void* pml4_virt = PageHeap::alloc();
        if (!pml4_virt) return false;

        memset(pml4_virt, 0, 4096);
        m_pml4 = (PML4Table*)pml4_virt;
        m_pml4_phys = virt_to_phys(pml4_virt);
        m_owns_pml4 = true;

        return true;
    }

    bool init_existing(PML4Table* pml4, uint64_t pml4_phys = 0) {
        m_pml4 = pml4;
        m_pml4_phys = pml4_phys ? pml4_phys : virt_to_phys(pml4);
        m_owns_pml4 = false;
        return true;
    }

    PML4Table* get_pml4() { return m_pml4; }
    uint64_t get_pml4_phys() { return m_pml4_phys; }

    struct ptrs {
        void *a, *b, *c, *d;

        void free() const {
            PageHeap::free(a);
            PageHeap::free(b);
            PageHeap::free(c);
            PageHeap::free(d);
        }
    };

    bool mmap(uint64_t virtual_addr, uint64_t physical_addr, const MappingFlags& flags = MappingFlags(), ptrs* ptrs = nullptr) {
        if (!validate_alignment(virtual_addr, physical_addr)) {
            return false;
        }

        struct ptrs tmpp;

        if (!ptrs) {
            ptrs = &tmpp;
        }

        VirtualAddress va{virtual_addr};
        PML4Entry& pml4ent = m_pml4->entries[va.MB2.pml4];

                if (!pml4ent.present || flags.force) {
            pml4ent.writable = 1;
            pml4ent.user = 1;
            pml4ent.nx = 0;
            pml4ent.pcd = !flags.cache;

            if (!pml4ent.present) {
                void* pdpt_virt = ptrs->a = PageHeap::alloc();
                if (!pdpt_virt) return false;

                memset(pdpt_virt, 0, 4096);
                uint64_t pdpt_phys = virt_to_phys(pdpt_virt);
                pml4ent.address = pdpt_phys >> 12;
            }
            pml4ent.present = 1;
        }

                uint64_t pdpt_phys = pml4ent.address << 12;
        void* pdpt_virt = phys_to_virt(pdpt_phys);
        PDPTTable* pdpt = (PDPTTable*)pdpt_virt;
        PDPTEntry& pdptent = pdpt->entries[va.MB2.pdpt];

        if (!pdptent.present || flags.force) {
            pdptent.writable = 1;
            pdptent.user = 1;
            pdptent.nx = 0;
            pdptent.pcd = !flags.cache;
            pdptent.page_size = 0;

            if (!pdptent.present) {
                void* pd_virt = ptrs->b = PageHeap::alloc();
                if (!pd_virt) return false;

                memset(pd_virt, 0, 4096);
                uint64_t pd_phys = virt_to_phys(pd_virt);
                pdptent.address = pd_phys >> 12;
            }
            pdptent.present = 1;
        }

                uint64_t pd_phys = pdptent.address << 12;
        void* pd_virt = phys_to_virt(pd_phys);
        PDTable* pd = (PDTable*)pd_virt;
        PDEntry& pdent = pd->entries[va.MB2.pd];

        if (!pdent.present || flags.force) {
            pdent.present = 1;
            pdent.writable = flags.writable ? 1 : 0;
            pdent.user = flags.user;
            pdent.nx = flags.execute ? 0 : 1;
            pdent.pcd = !flags.cache;
            pdent.page_size = 1;
            pdent.global = flags.global ? 1 : 0;
            pdent.address = physical_addr >> 12;

            invlpg(virtual_addr);
            return true;
        }

        return false;
    }

    bool mmap(uint64_t virtual_addr, uint64_t physical_addr,
              bool writable = true, bool user = true, bool execute = true,
              bool cache = false, bool global = false, bool force = true, bool create = true) {
        MappingFlags flags(writable, user, execute, cache, global, force, create);
        return mmap(virtual_addr, physical_addr, flags);
    }

    bool identity_map(uint64_t addr, const MappingFlags& flags = MappingFlags(), ptrs* ptrs = nullptr) {
        return mmap(addr, addr, flags, ptrs);
    }

    bool map_range_identity(uint64_t addr, uint64_t size, const MappingFlags& flags = MappingFlags()) {
        return map_range_custom(addr, addr, size, flags);
    }

    bool map_range_custom(uint64_t virt_start, uint64_t phys_start, uint64_t size,
                          const MappingFlags& flags = MappingFlags()) {
        for (uint64_t i = 0; i < size; i += 0x200000) {
            if (!mmap(virt_start + i, phys_start + i, flags)) return false;
        }
        return true;
    }

    void gmap(int offset, int giga, const MappingFlags& flags = MappingFlags()) {
        for (uint64_t i = 0; i < static_cast<uint64_t>(giga) * 512; i++) {
            identity_map(offset + i * 0x200000, flags);
        }
    }

    bool unmap(uint64_t virtual_addr) {
        VirtualAddress va{virtual_addr};
        PML4Entry& pml4ent = m_pml4->entries[va.MB2.pml4];

        if (!pml4ent.present) return false;

        uint64_t pdpt_phys = pml4ent.address << 12;
        void* pdpt_virt = phys_to_virt(pdpt_phys);
        PDPTTable* pdpt = (PDPTTable*)pdpt_virt;
        PDPTEntry& pdptent = pdpt->entries[va.MB2.pdpt];

        if (!pdptent.present) return false;

        uint64_t pd_phys = pdptent.address << 12;
        void* pd_virt = phys_to_virt(pd_phys);
        PDTable* pd = (PDTable*)pd_virt;
        PDEntry& pdent = pd->entries[va.MB2.pd];

        if (!pdent.present) return false;

        pdent.present = 0;
        invlpg(virtual_addr);

        return true;
    }

    bool switch_to() {
        if (!m_pml4_phys) return false;
        asm volatile("mov %0, %%cr3" : : "r"(m_pml4_phys) : "memory");
        return true;
    }

    static void set_kernel_manager(PagingManager* manager) {
        s_kernel_manager = manager;
    }

    static PagingManager* kernel() {
        return s_kernel_manager;
    }
};

struct paging {
    static PML4Table* pml4;

    static bool mmap(uint64_t virtual_addr, uint64_t physical_addr,
                     bool writable = true, bool user = true, bool execute = true,
                     bool cache = false, bool global = false, bool force = true, bool create = true) {
        if (!s_manager) init_default();
        return s_manager->mmap(virtual_addr, physical_addr, writable, user, execute, cache, global, force, create);
    }

    static bool identity_map(uint64_t addr) {
        return mmap(addr, addr);
    }

    static bool map_range_identity(uint64_t addr, uint64_t size) {
        return map_range_custom(addr, addr, size);
    }

    static bool map_range_custom(uint64_t virt_start, uint64_t phys_start, uint64_t size) {
        if (!s_manager) init_default();
        return s_manager->map_range_custom(virt_start, phys_start, size);
    }

    static void gmap(int offset, int giga) {
        if (!s_manager) init_default();
        s_manager->gmap(offset, giga);
    }

    static void init_default() {
        s_manager = new PagingManager();
        s_manager->init_existing((PML4Table*)PML4_DEFAULT);
        PagingManager::set_kernel_manager(s_manager);
    }

    static PagingManager* get_manager() { return s_manager; }

    static PagingManager* s_manager;
};

inline PML4Table* paging::pml4 = (PML4Table*)PML4_DEFAULT;
PagingManager* paging::s_manager = nullptr;
PagingManager* PagingManager::s_kernel_manager = nullptr;

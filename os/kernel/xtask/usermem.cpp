#pragma once

#include "../utils/inited.cpp"

namespace xmemory {
    struct allocator {
        PML4Table* pml4;
        PagingManager mg;
        MappingFlags cflags;
        MappingFlags mflags;
        managed<void*> ptrs;

        const uint64_t half = 0x100000;

        allocator(bool usermode = true): pml4((PML4Table*)PageHeap::alloc()), mg(pml4) {
            cflags = MappingFlags(
                false,
                usermode,
                true,
                false,
                false,
                true,
                false
            );
            mflags = MappingFlags(
                true,
                usermode,
                false,
                true,
                false,
                true,
                false
            );
        }

        void* cmalloc(size_t size) {
            void* res = ::malloc(size);
            PagingManager::ptrs p;
            mg.identity_map((uint64_t)res, cflags, &p);
            ptrs.push_back(res);

            if (p.a) ptrs.push_back(p.a);
            if (p.b) ptrs.push_back(p.b);
            if (p.c) ptrs.push_back(p.c);
            if (p.d) ptrs.push_back(p.d);

            return res;
        }

        void* mmalloc(size_t size) {
            void* res = ::malloc(size);
            PagingManager::ptrs p;
            mg.identity_map((uint64_t)res, mflags, &p);
            ptrs.push_back(res);

            if (p.a) ptrs.push_back(p.a);
            if (p.b) ptrs.push_back(p.b);
            if (p.c) ptrs.push_back(p.c);
            if (p.d) ptrs.push_back(p.d);

            return res;
        }

        ~allocator() {
            for (const auto ptr : ptrs) {
                PageHeap::free(ptr);
            }
        }
    };
}

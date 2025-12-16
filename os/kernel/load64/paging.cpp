#pragma once

#include "../types/types.cpp"
#include "../memory/memory.cpp"



struct PML4Entry {
    uint64_t present : 1;
    uint64_t rw : 1;
    uint64_t user : 1;
    uint64_t writethrough : 1;
    uint64_t cache : 1;
    uint64_t accessed : 1;
    uint64_t ignored1 : 1;
    uint64_t size : 1;
    uint64_t ignored2 : 4;
    uint64_t addr : 40;
    uint64_t ignored3 : 11;
    uint64_t xd : 1;
} __attribute__((packed));

void setup_64bit_paging() {


    uint64_t* pml4 = (uint64_t*)0x10000;


    uint64_t* pdpt = (uint64_t*)0x11000;


    uint64_t* pd = (uint64_t*)0x12000;


    uint64_t* pt = (uint64_t*)0x13000;


    memset(pml4, 0, 0x4000);




    for (int i = 0; i < 512; i++) {
        pt[i] = (i * 0x1000) | 0x003;
    }


    pd[0] = ((uint64_t)pt) | 0x003;


    pdpt[0] = ((uint64_t)pd) | 0x003;


    pml4[0] = ((uint64_t)pdpt) | 0x003;



    pml4[511] = ((uint64_t)pdpt) | 0x003;


    asm volatile("mov %0, %%cr3" : : "r"((uint32_t)pml4));
}
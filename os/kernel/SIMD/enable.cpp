#pragma once

#include "../types/types.cpp"


void enable_sse() {

    asm volatile("fninit");


    uint64_t cr0;
    asm volatile("mov %%cr0, %0" : "=r"(cr0));
    cr0 &= ~(1 << 2);
    cr0 |= (1 << 1);
    asm volatile("mov %0, %%cr0" :: "r"(cr0));


    uint64_t cr4;
    asm volatile("mov %%cr4, %0" : "=r"(cr4));
    cr4 |= (1 << 9);
    cr4 |= (1 << 10);
    asm volatile("mov %0, %%cr4" :: "r"(cr4));


    uint32_t mxcsr = 0x1F80;
    asm volatile("ldmxcsr %0" :: "m"(mxcsr));
}


uint64_t read_xcr(uint32_t xcr) {
    uint32_t eax, edx;
    asm volatile("xgetbv" : "=a"(eax), "=d"(edx) : "c"(xcr));
    return ((uint64_t)edx << 32) | eax;
}


void write_xcr(uint32_t xcr, uint64_t value) {
    uint32_t eax = (uint32_t)value;
    uint32_t edx = (uint32_t)(value >> 32);
    asm volatile("xsetbv" :: "c"(xcr), "a"(eax), "d"(edx));
}


bool check_avx_support() {
    uint32_t eax, ebx, ecx, edx;


    asm volatile("cpuid"
            : "=a"(eax), "=b"(ebx), "=c"(ecx), "=d"(edx)
            : "a"(1), "c"(0));



    return (ecx & (1 << 27)) && (ecx & (1 << 28));
}


bool check_avx512_support() {
    uint32_t eax, ebx, ecx, edx;


    asm volatile("cpuid"
            : "=a"(eax), "=b"(ebx), "=c"(ecx), "=d"(edx)
            : "a"(7), "c"(0));


    return (ebx & (1 << 16)) != 0;
}


void enable_avx() {

    enable_sse();


    if (!check_avx_support()) {

        return;
    }


    uint64_t cr4;
    asm volatile("mov %%cr4, %0" : "=r"(cr4));
    cr4 |= (1 << 18);
    asm volatile("mov %0, %%cr4" :: "r"(cr4));


    uint64_t xcr0 = read_xcr(0);


    xcr0 |= (1 << 0);
    xcr0 |= (1 << 1);
    xcr0 |= (1 << 2);


    if (check_avx512_support()) {

        xcr0 |= (1 << 5);
        xcr0 |= (1 << 6);
        xcr0 |= (1 << 7);
    }


    write_xcr(0, xcr0);


    asm volatile(
            "vzeroall\n\t"
            :
            :
            : "ymm0", "ymm1", "ymm2", "ymm3",
    "ymm4", "ymm5", "ymm6", "ymm7",
    "ymm8", "ymm9", "ymm10", "ymm11",
    "ymm12", "ymm13", "ymm14", "ymm15"
            );
}


void init_fpu_extensions() {



    asm volatile("fninit");


    enable_sse();


    enable_avx();


    /*

    if (check_avx_support()) {

        float test_data[8] = {1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f};
        asm volatile(
            "vmovups %0, %%ymm0\n\t"
            "vmovups %%ymm0, %0\n\t"
            : "+m"(test_data)
            :
            : "ymm0"
        );
    }
    */
}
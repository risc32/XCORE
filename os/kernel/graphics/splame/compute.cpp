
#pragma once

#define splamecount 4096

#include "../../utils/utils.cpp"

void _sp_bitcount(__uint128_t* map, uint8_t* result) {

    __uint128_t* src = map;
    uint8_t* dst = result;


    asm volatile (
        "mov %0, %%rsi\n"
        "mov %1, %%rdi\n"
        "mov $256, %%ecx\n"

        ".Lmain_loop:\n"


        "mov (%%rsi), %%rax\n"
        "mov 8(%%rsi), %%rdx\n"
        "popcnt %%rax, %%rax\n"
        "popcnt %%rdx, %%rdx\n"
        "add %%rdx, %%rax\n"
        "mov %%al, (%%rdi)\n"


        "mov 16(%%rsi), %%rax\n"
        "mov 24(%%rsi), %%rdx\n"
        "popcnt %%rax, %%rax\n"
        "popcnt %%rdx, %%rdx\n"
        "add %%rdx, %%rax\n"
        "mov %%al, 1(%%rdi)\n"


        "mov 32(%%rsi), %%rax\n"
        "mov 40(%%rsi), %%rdx\n"
        "popcnt %%rax, %%rax\n"
        "popcnt %%rdx, %%rdx\n"
        "add %%rdx, %%rax\n"
        "mov %%al, 2(%%rdi)\n"


        "mov 48(%%rsi), %%rax\n"
        "mov 56(%%rsi), %%rdx\n"
        "popcnt %%rax, %%rax\n"
        "popcnt %%rdx, %%rdx\n"
        "add %%rdx, %%rax\n"
        "mov %%al, 3(%%rdi)\n"


        "mov 64(%%rsi), %%rax\n"
        "mov 72(%%rsi), %%rdx\n"
        "popcnt %%rax, %%rax\n"
        "popcnt %%rdx, %%rdx\n"
        "add %%rdx, %%rax\n"
        "mov %%al, 4(%%rdi)\n"


        "mov 80(%%rsi), %%rax\n"
        "mov 88(%%rsi), %%rdx\n"
        "popcnt %%rax, %%rax\n"
        "popcnt %%rdx, %%rdx\n"
        "add %%rdx, %%rax\n"
        "mov %%al, 5(%%rdi)\n"


        "mov 96(%%rsi), %%rax\n"
        "mov 104(%%rsi), %%rdx\n"
        "popcnt %%rax, %%rax\n"
        "popcnt %%rdx, %%rdx\n"
        "add %%rdx, %%rax\n"
        "mov %%al, 6(%%rdi)\n"


        "mov 112(%%rsi), %%rax\n"
        "mov 120(%%rsi), %%rdx\n"
        "popcnt %%rax, %%rax\n"
        "popcnt %%rdx, %%rdx\n"
        "add %%rdx, %%rax\n"
        "mov %%al, 7(%%rdi)\n"


        "mov 128(%%rsi), %%rax\n"
        "mov 136(%%rsi), %%rdx\n"
        "popcnt %%rax, %%rax\n"
        "popcnt %%rdx, %%rdx\n"
        "add %%rdx, %%rax\n"
        "mov %%al, 8(%%rdi)\n"


        "mov 144(%%rsi), %%rax\n"
        "mov 152(%%rsi), %%rdx\n"
        "popcnt %%rax, %%rax\n"
        "popcnt %%rdx, %%rdx\n"
        "add %%rdx, %%rax\n"
        "mov %%al, 9(%%rdi)\n"


        "mov 160(%%rsi), %%rax\n"
        "mov 168(%%rsi), %%rdx\n"
        "popcnt %%rax, %%rax\n"
        "popcnt %%rdx, %%rdx\n"
        "add %%rdx, %%rax\n"
        "mov %%al, 10(%%rdi)\n"


        "mov 176(%%rsi), %%rax\n"
        "mov 184(%%rsi), %%rdx\n"
        "popcnt %%rax, %%rax\n"
        "popcnt %%rdx, %%rdx\n"
        "add %%rdx, %%rax\n"
        "mov %%al, 11(%%rdi)\n"


        "mov 192(%%rsi), %%rax\n"
        "mov 200(%%rsi), %%rdx\n"
        "popcnt %%rax, %%rax\n"
        "popcnt %%rdx, %%rdx\n"
        "add %%rdx, %%rax\n"
        "mov %%al, 12(%%rdi)\n"


        "mov 208(%%rsi), %%rax\n"
        "mov 216(%%rsi), %%rdx\n"
        "popcnt %%rax, %%rax\n"
        "popcnt %%rdx, %%rdx\n"
        "add %%rdx, %%rax\n"
        "mov %%al, 13(%%rdi)\n"


        "mov 224(%%rsi), %%rax\n"
        "mov 232(%%rsi), %%rdx\n"
        "popcnt %%rax, %%rax\n"
        "popcnt %%rdx, %%rdx\n"
        "add %%rdx, %%rax\n"
        "mov %%al, 14(%%rdi)\n"


        "mov 240(%%rsi), %%rax\n"
        "mov 248(%%rsi), %%rdx\n"
        "popcnt %%rax, %%rax\n"
        "popcnt %%rdx, %%rdx\n"
        "add %%rdx, %%rax\n"
        "mov %%al, 15(%%rdi)\n"


        "add $256, %%rsi\n"
        "add $16, %%rdi\n"
        "dec %%ecx\n"
        "jnz .Lmain_loop\n"

        :
        : "r" (src), "r" (dst)
        : "rax", "rdx", "rcx", "rsi", "rdi", "cc", "memory"
    );
}
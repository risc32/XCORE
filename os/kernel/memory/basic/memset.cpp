#pragma once

#include "../../types/scalar.cpp"

void memset(void *dest, int value, size_t count) {
    auto *ptr = (unsigned char *) dest;
    auto byte_value = (unsigned char) value;

    if (count >= 4) {
        uint32_t word_value = byte_value | (byte_value << 8) |
                              (byte_value << 16) | (byte_value << 24);

        while (((uintptr_t) ptr & 3) && count > 0) {
            *ptr++ = byte_value;
            count--;
        }

        auto *word_ptr = (uint32_t *) ptr;
        while (count >= 4) {
            *word_ptr++ = word_value;
            count -= 4;
        }

        ptr = (unsigned char *) word_ptr;
    }

    while (count > 0) {
        *ptr++ = byte_value;
        count--;
    }

}

void memzero(void *dest, size_t count) {
    if (count >= 64) {

        asm volatile (
                "cld\n\t"
                "mov %0, %%edi\n\t"
                "mov %1, %%ecx\n\t"
                "xor %%eax, %%eax\n\t"
                "rep stosb\n\t"
                :
                : "r" (dest), "r" (count)
                : "eax", "ecx", "edi", "memory"
                );
    } else {

        memset(dest, 0, count);
    }
}
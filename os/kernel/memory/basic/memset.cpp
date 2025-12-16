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

    for (size_t i = 0; i < count; i++) {
        ((char*)dest)[i] = 0x00000000;
    }
}
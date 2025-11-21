#pragma once

#include "../types/scalar.cpp"

size_t strlen(const char *str) {
    if (!str) {
        return 0;
    }

    size_t length = 0;
    while (str[length] != '\0') {
        length++;
    }

    return length;
}

int count_digits(int num, int base = 10) {
    if (num == 0) return 1;

    int count = 0;
    while (num != 0) {
        num /= base;
        count++;
    }
    return count;
}
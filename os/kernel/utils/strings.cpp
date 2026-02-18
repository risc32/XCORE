#pragma once

#include "../types/types.cpp"


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

void strcpy(char *dest, const char *src) {
    size_t i = 0;
    while (src[i] != '\0') {
        dest[i] = src[i];
        i++;
    }
    dest[i] = '\0';
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


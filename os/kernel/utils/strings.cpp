#pragma once

#include "../types/types.cpp"

extern "C" {

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

int strcmp(const char *s1, const char *s2) {
    size_t i = 0;
    while (s1[i] != '\0' || s2[i] != '\0') {
        if (s1[i] != s2[i]) {
            return (unsigned char)s1[i] - (unsigned char)s2[i];
        }
        i++;
    }
    return 0;
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

char* strchr(const char* s, int c) {
    while (*s) {
        if (*s == (char)c) {
            return (char*)s;
        }
        s++;
    }
    /* Check for null terminator */
    if (c == '\0') {
        return (char*)s;
    }
    return nullptr;
}
}
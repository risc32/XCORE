#pragma once

char digit_to_char(int digit) {
    if (digit >= 0 && digit <= 9) return '0' + digit;
    if (digit >= 10 && digit <= 35) return 'A' + (digit - 10);
    return '?';
}

char *itoa(int value, char *buffer, int base) {
    if (base < 2 || base > 36) {
        buffer[0] = '\0';
        return buffer;
    }

    if (value == 0) {
        buffer[0] = '0';
        buffer[1] = '\0';
        return buffer;
    }

    char temp_buffer[32];
    int index = 0;
    bool negative = false;

    unsigned int unsigned_value;
    if (value < 0 && base == 10) {
        negative = true;
        unsigned_value = (unsigned int) (-value);
    } else {
        unsigned_value = (unsigned int) value;
    }

    do {
        int digit = unsigned_value % base;
        temp_buffer[index++] = digit_to_char(digit);
        unsigned_value /= base;
    } while (unsigned_value != 0);

    if (negative) {
        temp_buffer[index++] = '-';
    }

    int buf_index = 0;
    for (int i = index - 1; i >= 0; i--) {
        buffer[buf_index++] = temp_buffer[i];
    }
    buffer[buf_index] = '\0';

    return buffer;
}

char *itoa_dec(int value, char *buffer) {
    return itoa(value, buffer, 10);
}

char *itoa_hex(int value, char *buffer) {
    return itoa(value, buffer, 16);
}

char *itoa_bin(int value, char *buffer) {
    return itoa(value, buffer, 2);
}

char *utoa(unsigned int value, char *buffer, int base) {
    if (base < 2 || base > 36) {
        buffer[0] = '\0';
        return buffer;
    }

    if (value == 0) {
        buffer[0] = '0';
        buffer[1] = '\0';
        return buffer;
    }

    char temp_buffer[32];
    int index = 0;

    do {
        int digit = value % base;
        temp_buffer[index++] = digit_to_char(digit);
        value /= base;
    } while (value != 0);

    int buf_index = 0;
    for (int i = index - 1; i >= 0; i--) {
        buffer[buf_index++] = temp_buffer[i];
    }
    buffer[buf_index] = '\0';

    return buffer;
}
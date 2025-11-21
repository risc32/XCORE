#pragma once

int char_to_digit(char c) {
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'a' && c <= 'z') return c - 'a' + 10;
    if (c >= 'A' && c <= 'Z') return c - 'A' + 10;
    return -1;
}

int str_to_int(const char *str, char **endptr, int base) {
    while (*str == ' ' || *str == '\t' || *str == '\n') {
        str++;
    }

    int sign = 1;
    if (*str == '-') {
        sign = -1;
        str++;
    } else if (*str == '+') {
        str++;
    }

    if (base == 0) {
        if (*str == '0') {
            if (*(str + 1) == 'x' || *(str + 1) == 'X') {
                base = 16;
                str += 2;
            } else {
                base = 8;
                str++;
            }
        } else {
            base = 10;
        }
    }

    if (base == 16 && *str == '0' && (*(str + 1) == 'x' || *(str + 1) == 'X')) {
        str += 2;
    }

    int result = 0;

    while (*str != '\0') {
        int digit = char_to_digit(*str);

        if (digit < 0 || digit >= base) {
            break;
        }

        if (result > (0x7FFFFFFF - digit) / base) {
            result = 0x7FFFFFFF;
            break;
        }

        result = result * base + digit;
        str++;
    }

    if (endptr != nullptr) {
        *endptr = (char *) str;
    }

    return sign * result;
}

int atoi(const char *str, int base = 10) {
    return str_to_int(str, nullptr, base);
}

int atoi_hex(const char *str) {
    return str_to_int(str, nullptr, 16);
}

int atoi_bin(const char *str) {
    return str_to_int(str, nullptr, 2);
}
#pragma once

char digit_to_char(int digit) {
    if (digit >= 0 && digit <= 9) return '0' + digit;
    if (digit >= 10 && digit <= 35) return 'A' + (digit - 10);
    return '?';
}


template<typename T>
char* generic_itoa(T value, char* buffer, int base) {
    if (base < 2 || base > 36) {
        buffer[0] = '\0';
        return buffer;
    }

    if (value == 0) {
        buffer[0] = '0';
        buffer[1] = '\0';
        return buffer;
    }


    char temp_buffer[sizeof(T) * 8 + 2];
    int index = 0;
    bool negative = false;


    typedef long long unsigned_type;
    unsigned_type unsigned_value;

    if (value < 0 && base == 10) {
        negative = true;
        unsigned_value = (unsigned_type)(-value);
    } else {
        unsigned_value = (unsigned_type)value;
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


template<typename T>
char* generic_utoa(T value, char* buffer, int base) {
    if (base < 2 || base > 36) {
        buffer[0] = '\0';
        return buffer;
    }

    if (value == 0) {
        buffer[0] = '0';
        buffer[1] = '\0';
        return buffer;
    }

    char temp_buffer[sizeof(T) * 8 + 1];
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


template<typename T> struct make_unsigned;
template<> struct make_unsigned<signed char> { typedef unsigned char type; };
template<> struct make_unsigned<unsigned char> { typedef unsigned char type; };
template<> struct make_unsigned<signed short> { typedef unsigned short type; };
template<> struct make_unsigned<unsigned short> { typedef unsigned short type; };
template<> struct make_unsigned<signed int> { typedef unsigned int type; };
template<> struct make_unsigned<unsigned int> { typedef unsigned int type; };
template<> struct make_unsigned<signed long> { typedef unsigned long type; };
template<> struct make_unsigned<unsigned long> { typedef unsigned long type; };
template<> struct make_unsigned<signed long long> { typedef unsigned long long type; };
template<> struct make_unsigned<unsigned long long> { typedef unsigned long long type; };


inline char* itoa(int value, char* buffer, int base) {
    return generic_itoa<int>(value, buffer, base);
}

inline char* ltoa(long value, char* buffer, int base) {
    return generic_itoa<long>(value, buffer, base);
}

inline char* lltoa(long long value, char* buffer, int base) {
    return generic_itoa<long long>(value, buffer, base);
}

inline char* utoa(unsigned int value, char* buffer, int base) {
    return generic_utoa<unsigned int>(value, buffer, base);
}

inline char* ultoa(unsigned long value, char* buffer, int base) {
    return generic_utoa<unsigned long>(value, buffer, base);
}

inline char* ulltoa(unsigned long long value, char* buffer, int base) {
    return generic_utoa<unsigned long long>(value, buffer, base);
}


#define DECLARE_HELPERS(type, prefix, sign) \
    inline char* prefix##toa_dec(type value, char* buffer) { \
        return generic_##sign##toa<type>(value, buffer, 10); \
    } \
    inline char* prefix##toa_hex(type value, char* buffer) { \
        return generic_##sign##toa<type>(value, buffer, 16); \
    } \
    inline char* prefix##toa_bin(type value, char* buffer) { \
        return generic_##sign##toa<type>(value, buffer, 2); \
    }

DECLARE_HELPERS(int, i, i)
DECLARE_HELPERS(long, l, i)
DECLARE_HELPERS(long long, ll, i)
DECLARE_HELPERS(unsigned int, u, u)
DECLARE_HELPERS(unsigned long, ul, u)
DECLARE_HELPERS(unsigned long long, ull, u)


//inline char *itoa_dec(int value, char *buffer) {

//}
//
//inline char *itoa_hex(int value, char *buffer) {

//}
//
//inline char *itoa_bin(int value, char *buffer) {

//}
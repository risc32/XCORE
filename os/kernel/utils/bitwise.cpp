#pragma once

static bool get_bit(const char* block, int bit_position) {
    int byte_index = bit_position / 8;
    int bit_index = bit_position % 8;

    return (block[byte_index] >> bit_index) & 1;
}

static void set_bit(char* block, int bit_position, bool value = true) {
    int byte_index = bit_position / 8;
    int bit_index = bit_position % 8;

    if (value) {
        block[byte_index] |= (1 << bit_index);
    } else {
        block[byte_index] &= ~(1 << bit_index);
    }
}
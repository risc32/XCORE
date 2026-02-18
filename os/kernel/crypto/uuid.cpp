#pragma once

#include "../utils/utils.cpp"
#include "random.cpp"

struct uuid_t;
string to_string(uuid_t uuid);

struct uuid_t {
    union {
        uint8_t  b[16];
        uint32_t dw[4];
        uint64_t qw[2];
    };

    string str() {
        return to_string(*this);
    }
};

#define UUID_VERSION_4 0x40
#define UUID_VARIANT_RFC 0x80

uuid_t uuid4() {
    uuid_t out;

    for (int i = 0; i < 16; i++) {
        out.b[i] = krand.random8();
    }



    out.b[6] = (out.b[6] & 0x0F) | UUID_VERSION_4;



    out.b[8] = (out.b[8] & 0x3F) | UUID_VARIANT_RFC;
    return out;
}

string to_string(uuid_t uuid) {

    string out;
    for (int i = 0; i < 16; i++) {
        out += to_string(uuid.b[i] >> 4, 16);
        out += to_string(uuid.b[i] & 0x0F, 16);
        if (i == 3 || i == 5 || i == 7 || i == 9) {
            out += "-";
        }
    }
    return out;
}
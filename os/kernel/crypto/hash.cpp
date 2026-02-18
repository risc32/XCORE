#include "../utils/inited.cpp"

namespace hash {


    uint64_t prime(const char* name) {

        const uint64_t P1 = 0x9E3779B97F4A7C15;
        const uint64_t P2 = 0xBF58476D1CE4E5B9;
        const uint64_t P3 = 0x94D049BB133111EB;

        uint64_t hash = P1;
        size_t len = 0;

        for (const char* p = name; *p; p++, len++) {
            hash ^= (uint64_t)(*p) << (len & 56);
            hash *= P2;
            hash += P3;


            hash = (hash >> 23) ^ (hash << 41);
        }


        hash ^= len * P3;
        hash ^= hash >> 29;
        hash *= P1;
        hash ^= hash >> 32;

        return hash;
    }

    uint64_t stream(const char* name) {
        uint64_t state[4] = {
            0x123456789ABCDEF,
            0x23456789ABCDEF1,
            0x3456789ABCDEF12,
            0x456789ABCDEF123
        };

        for (const char* p = name; *p; p++) {
            uint8_t c = *p;


            state[0] ^= c;
            state[1] += state[0];
            state[2] ^= state[1] << 17;
            state[3] += state[2] >> 13;


            uint64_t t = state[0] ^ (state[1] * 0x9E3779B9);
            state[0] = state[2] ^ (state[3] + t);
            state[1] = state[3] ^ (state[0] - t);
            state[2] = t ^ (state[1] << 23);
            state[3] = t ^ (state[2] >> 47);
        }


        return state[0] ^ state[1] ^ state[2] ^ state[3];
    }

    __int128 _highprec1(const char* name) {
        return prime(name) << 32 | stream(name);
    }
}
#include "../managed/managed.cpp"

class Random {
    uint32_t state[16];
    uint8_t  buffer[64];
    int      pos;


    static inline uint32_t rotl(uint32_t x, int n) {
        return (x << n) | (x >> (32 - n));
    }


    void quarter_round(uint32_t& a, uint32_t& b, uint32_t& c, uint32_t& d) {
        a += b; d ^= a; d = rotl(d, 16);
        c += d; b ^= c; b = rotl(b, 12);
        a += b; d ^= a; d = rotl(d, 8);
        c += d; b ^= c; b = rotl(b, 7);
    }


    void next_block() {
        uint32_t x[16];
        for (int i = 0; i < 16; i++) x[i] = state[i];

        for (int i = 0; i < 10; i++) {
            quarter_round(x[0], x[4], x[8],  x[12]);
            quarter_round(x[1], x[5], x[9],  x[13]);
            quarter_round(x[2], x[6], x[10], x[14]);
            quarter_round(x[3], x[7], x[11], x[15]);
            quarter_round(x[0], x[5], x[10], x[15]);
            quarter_round(x[1], x[6], x[11], x[12]);
            quarter_round(x[2], x[7], x[8],  x[13]);
            quarter_round(x[3], x[4], x[9],  x[14]);
        }

        for (int i = 0; i < 16; i++) {
            x[i] += state[i];
        }

        uint8_t* out = buffer;
        for (int i = 0; i < 16; i++) {
            out[0] = x[i] & 0xFF;
            out[1] = (x[i] >> 8) & 0xFF;
            out[2] = (x[i] >> 16) & 0xFF;
            out[3] = (x[i] >> 24) & 0xFF;
            out += 4;
        }

        state[12]++;
        if (state[12] == 0) state[13]++;

        pos = 0;
    }


    bool rdrand_available() {
        uint32_t eax, ebx, ecx, edx;
        __asm__ volatile("cpuid" : "=a"(eax), "=b"(ebx), "=c"(ecx), "=d"(edx) : "a"(1));
        return (ecx >> 30) & 1;
    }


    bool rdrand32(uint32_t* val) {
        unsigned char ok;
        __asm__ volatile("rdrand %0 ; setc %1" : "=r"(*val), "=qm"(ok));
        return ok;
    }

public:
    static void init();



    Random() {
        pos = 64;


        state[0] = 0x61707865;
        state[1] = 0x3320646e;
        state[2] = 0x79622d32;
        state[3] = 0x6b206574;


        state[12] = 0;

        if (rdrand_available()) {

            for (int i = 0; i < 8; i++) {
                if (!rdrand32(&state[4 + i])) {

                    state[4 + i] = 0;
                }
            }
            for (int i = 0; i < 3; i++) {
                if (!rdrand32(&state[13 + i])) {
                    state[13 + i] = 0;
                }
            }
        } else {

            for (int i = 4; i < 12; i++) state[i] = 0;
            state[13] = state[14] = state[15] = 0;
        }
    }




    Random(const uint32_t key[8], const uint32_t nonce[3]) {

        state[0] = 0x61707865;
        state[1] = 0x3320646e;
        state[2] = 0x79622d32;
        state[3] = 0x6b206574;


        for (int i = 0; i < 8; i++) state[4 + i] = key[i];


        state[12] = 0;


        state[13] = nonce[0];
        state[14] = nonce[1];
        state[15] = nonce[2];

        pos = 64;
    }




    void seed(const uint32_t key[8], const uint32_t nonce[3]) {

        state[0] = 0x61707865;
        state[1] = 0x3320646e;
        state[2] = 0x79622d32;
        state[3] = 0x6b206574;


        for (int i = 0; i < 8; i++) state[4 + i] = key[i];


        state[12] = 0;


        state[13] = nonce[0];
        state[14] = nonce[1];
        state[15] = nonce[2];

        pos = 64;
    }




    __int128 random128() {
        __int128 res = 0;

        if (pos >= 64) next_block();
        res |= *(uint64_t*)(buffer + pos);
        pos += 8;

        if (pos >= 64) next_block();
        res |= *(uint64_t*)(buffer + pos) << 64;
        pos += 8;

        return res;
    }




    uint64_t random64() {
        if (pos >= 64) next_block();
        uint64_t val = *(uint64_t*)(buffer + pos);
        pos += 8;
        return val;
    }




    uint32_t random32() {
        if (pos >= 64) next_block();
        uint32_t val = *(uint32_t*)(buffer + pos);
        pos += 4;
        return val;
    }




    uint16_t random16() {
        if (pos >= 64) next_block();
        uint16_t val = *(uint16_t*)(buffer + pos);
        pos += 2;
        return val;
    }




    uint8_t random8() {
        if (pos >= 64) next_block();
        return buffer[pos++];
    }
};

Random krand;

void Random::init() {
    krand = Random();
}

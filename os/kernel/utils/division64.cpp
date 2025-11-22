#include "../types/types.cpp"

extern "C" uint64_t __udivdi3(uint64_t a, uint64_t b) {

    if (b == 0) {

        return 0xFFFFFFFFFFFFFFFF;
    }
    if (b > a) return 0;
    if (b == a) return 1;

    uint64_t res = 0;
    uint64_t denominator = b;
    uint64_t current = 1;


    while (denominator <= a && !(denominator & (1ULL << 63))) {
        denominator <<= 1;
        current <<= 1;
    }


    while (current > 0) {
        if (a >= denominator) {
            a -= denominator;
            res |= current;
        }
        denominator >>= 1;
        current >>= 1;
    }

    return res;
}

extern "C" uint64_t __umoddi3(uint64_t a, uint64_t b) {
    if (b == 0) return a;
    return a - __udivdi3(a, b) * b;
}


extern "C" int64_t __divdi3(int64_t a, int64_t b) {
    bool negative = (a < 0) != (b < 0);
    uint64_t ua = a < 0 ? -a : a;
    uint64_t ub = b < 0 ? -b : b;
    uint64_t result = __udivdi3(ua, ub);
    return negative ? -result : result;
}

extern "C" int64_t __moddi3(int64_t a, int64_t b) {
    uint64_t ua = a < 0 ? -a : a;
    uint64_t ub = b < 0 ? -b : b;
    uint64_t result = __umoddi3(ua, ub);
    return a < 0 ? -result : result;
}
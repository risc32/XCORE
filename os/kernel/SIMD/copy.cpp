#include "../utils/utils.cpp"

#ifdef __x86_64__
#define REG_PREFIX "%"
#else
#define REG_PREFIX "%%"
#endif

namespace simd {
    template<size_t N>
    void copy(const void* src, void* dst) {
        if constexpr (N == 16) {
            __asm__ volatile (
                    "movdqu (%1), %%xmm0\n\t"
                    "movdqu %%xmm0, (%0)"
                    :
                    : "r"(dst), "r"(src)
                    : "xmm0", "memory"
                    );
        } else if constexpr (N == 32) {
            __asm__ volatile (
                    "vmovdqu (%1), %%ymm0\n\t"  /
                    "vmovdqu %%ymm0, (%0)"
                    :
                    : "r"(dst), "r"(src)
                    : "ymm0", "memory"
                    );
        } else if constexpr (N == 64) {
            __asm__ volatile (
                    "vmovdqu64 (%1), %%zmm0\n\t" /
                    "vmovdqu64 %%zmm0, (%0)"
                    :
                    : "r"(dst), "r"(src)
                    : "zmm0", "memory"
                    );
        }
    }

    void copy128(const void* src, void* dst) {
        copy<16>(src, dst);
    }

    void copy256(const void* src, void* dst) {
        copy<32>(src, dst);
    }

    void copy512(const void* src, void* dst) {
        copy<64>(src, dst);
    }
}
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
                    "movdqa (%1), %%xmm0\n\t"
                    "movdqa %%xmm0, (%0)"
                    :
                    : "r"(dst), "r"(src)
                    : "xmm0", "memory"
                    );
        } else if constexpr (N == 32) {
            __asm__ volatile (
                    "vmovdqa (%1), %%ymm0\n\t"
                    "vmovdqa %%ymm0, (%0)"
                    :
                    : "r"(dst), "r"(src)
                    : "ymm0", "memory"
                    );
        } else if constexpr (N == 64) {
            __asm__ volatile (
                    "vmovdqa64 (%1), %%zmm0\n\t"
                    "vmovdqa64 %%zmm0, (%0)"
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

    void copy(void* dst, const void* src, size_t size) {
        uint8_t* d = (uint8_t*)dst;
        const uint8_t* s = (const uint8_t*)src;


        if (size > 1024 * 1024) {

            size_t i = 0;
            for (; i + 64 <= size; i += 64) {
                asm volatile (
                        "movdqa (%1), %%xmm0\n\t"
                        "movdqa 16(%1), %%xmm1\n\t"
                        "movdqa 32(%1), %%xmm2\n\t"
                        "movdqa 48(%1), %%xmm3\n\t"
                        "movntdq %%xmm0, (%0)\n\t"
                        "movntdq %%xmm1, 16(%0)\n\t"
                        "movntdq %%xmm2, 32(%0)\n\t"
                        "movntdq %%xmm3, 48(%0)\n\t"
                        :
                        : "r"(d + i), "r"(s + i)
                        : "xmm0", "xmm1", "xmm2", "xmm3", "memory"
                        );
            }
            asm volatile("sfence" ::: "memory");
            d += i; s += i; size -= i;
        }


        while (size >= 64) {
            asm volatile (
                    "movdqa (%1), %%xmm0\n\t"
                    "movdqa 16(%1), %%xmm1\n\t"
                    "movdqa 32(%1), %%xmm2\n\t"
                    "movdqa 48(%1), %%xmm3\n\t"
                    "movdqa %%xmm0, (%0)\n\t"
                    "movdqa %%xmm1, 16(%0)\n\t"
                    "movdqa %%xmm2, 32(%0)\n\t"
                    "movdqa %%xmm3, 48(%0)\n\t"
                    :
                    : "r"(d), "r"(s)
                    : "xmm0", "xmm1", "xmm2", "xmm3", "memory"
                    );
            d += 64; s += 64; size -= 64;
        }


        while (size >= 8) {
            *(uint64_t*)d = *(const uint64_t*)s;
            d += 8; s += 8; size -= 8;
        }
        while (size--) *d++ = *s++;
    }
}
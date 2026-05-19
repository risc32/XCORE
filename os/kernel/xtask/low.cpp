#pragma once

#include <utils>

namespace smplow {
    uint32_t* lapic = (uint32_t*)0xFEE00000;

    uint32_t get_core_id() {
        lapic = (uint32_t*)0xFEE00000;
        return lapic[0x20 / 4] >> 24;
    }

    void setstack(uint64_t sp) {
        asm volatile("mov %0, %%rsp" : : "r"(sp));
    }
}

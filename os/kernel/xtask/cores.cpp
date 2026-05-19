#pragma once

#include <utils>
#include "const.cpp"
#include "../disk/disk.cpp"
#include "xtask.cpp"

namespace cores {
    uint32_t* lapic = (uint32_t*)0xFEE00000;
    int c;

    void ap_entry() {
        s0::put("Privet\n");

        stop();
    }

    void startup() {
        disk::read(1, 2, (char*)0x7000);

        uint32_t bsp_id = lapic[LAPIC_ID / 4] >> 24;
        uint8_t vector = 0x07;
        *(uint64_t*)0x100000 = (uint64_t)xtask::setup;

        for (uint32_t apic_id = 1; apic_id < 16; apic_id++) {
            if (apic_id == bsp_id) continue;

            lapic[LAPIC_ICR_HI / 4] = apic_id << 24;
            lapic[LAPIC_ICR_LO / 4] = 0x4500;
            for (volatile int i = 0; i < 100000; i++){}

            lapic[LAPIC_ICR_LO / 4] = 0x4500 & ~(1 << 14);
            for (volatile int i = 0; i < 10000; i++){}

            for (int s = 0; s < 2; s++) {
                lapic[LAPIC_ICR_HI / 4] = apic_id << 24;
                lapic[LAPIC_ICR_LO / 4] = 0x4600 | vector;
                for (volatile int i = 0; i < 10000; i++);
            }
        }
    }

    xtask::Core* find_best() {
        xtask::Core* m = &xtask::cores[0];
        for (auto& core : xtask::cores) {
            if (core.tasks.size() < m->tasks.size()) {
                m = &core;
            }
        }
        return m;
    }
}


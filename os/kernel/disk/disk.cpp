#pragma once

#include "atadriver.cpp"

enum cltype {
    dir,
    file
};

struct cluster {
    cltype type;

};

struct disk {
    static ATADriver driver;

    static void init() {

        driver = {};
        s0::put("void disk::init() KERNEL 0x20000 .text\n");

    }

    static void read(uint64_t lba, uint32_t count, char *buffer) {
        driver.read(lba, count, buffer);
    };

    static managed<char> read(uint64_t lba, uint32_t count) {
        return driver.read(lba, count);
    };

    static bool write(uint64_t lba, const char *data) {
        return driver.write(lba, data);
    }
};

ATADriver disk::driver = {};
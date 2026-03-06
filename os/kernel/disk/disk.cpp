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

    static bool write(uint64_t lba, uint32_t count, const char *data) {
        for (int i = 0; i < count; i++) {
            if (!driver.write(lba + i, data + (i * 512)))
                return false;
        }
        return true;
    }

    static bool write(uint64_t lba, const managed<char>& data, uint64_t seek = 0)
    {
        if (data.size() == 0) return true;


        uint64_t start_lba = lba + (seek / 512);
        uint64_t offset_in_first = seek % 512;
        uint64_t bytes_remaining = data.size();
        uint64_t data_pos = 0;


        char sector_buffer[512];


        if (offset_in_first > 0) {

            read(start_lba, 1, sector_buffer);


            uint64_t bytes_to_write = min(bytes_remaining, 512 - offset_in_first);


            memcpy(sector_buffer + offset_in_first, data.data() + data_pos, bytes_to_write);


            if (!driver.write(start_lba, sector_buffer))
                return false;

            data_pos += bytes_to_write;
            bytes_remaining -= bytes_to_write;
            start_lba++;
        }


        while (bytes_remaining >= 512) {
            if (!driver.write(start_lba, data.data() + data_pos))
                return false;

            data_pos += 512;
            bytes_remaining -= 512;
            start_lba++;
        }


        if (bytes_remaining > 0) {

            read(start_lba, 1, sector_buffer);


            memcpy(sector_buffer, data.data() + data_pos, bytes_remaining);


            if (!driver.write(start_lba, sector_buffer))
                return false;
        }

        return true;
    }
};

ATADriver disk::driver = {};
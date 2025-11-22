#pragma once

#include "../types/types.cpp"
#include "../cpu/cpu.cpp"

struct DiskGeometry {
    uint64_t total_sectors;
    uint32_t sector_size;
    bool lba48_supported;
    uint64_t total_bytes;
};

class ATADriver {
public:
    ATADriver(uint16_t port = 0x1F0) : base_port(port) {
        detect_capabilities();
        geometry = detect_geometry();
    }

    void detect_capabilities() {
        outb(base_port + 6, 0xA0);
        outb(base_port + 2, 0);
        outb(base_port + 3, 0);
        outb(base_port + 4, 0);
        outb(base_port + 5, 0);
        outb(base_port + 7, 0xEC);

        if (inb(base_port + 7) == 0) {
            lba48_supported = false;
            return;
        }

        wait_bsy();

        uint16_t identify[256];
        for (int i = 0; i < 256; i++) {
            identify[i] = inw(base_port);
        }
        lba48_supported = identify[83] & (1 << 10);
    }

    DiskGeometry detect_geometry() {
        DiskGeometry geo = {};
        geo.sector_size = 512;
        geo.lba48_supported = lba48_supported;

        if (!lba48_supported) {

            geo.total_sectors = (1ULL << 28) - 1;
        } else {

            outb(base_port + 6, 0xA0);
            outb(base_port + 2, 0);
            outb(base_port + 3, 0);
            outb(base_port + 4, 0);
            outb(base_port + 5, 0);
            outb(base_port + 7, 0xEC);

            wait_bsy();

            uint16_t identify[256];
            for (int i = 0; i < 256; i++) {
                identify[i] = inw(base_port);
            }


            geo.total_sectors =
                    (uint64_t)identify[100] |
                    ((uint64_t)identify[101] << 16) |
                    ((uint64_t)identify[102] << 32) |
                    ((uint64_t)identify[103] << 48);


            if (geo.total_sectors == 0) {
                geo.total_sectors =
                        (uint64_t)identify[60] |
                        ((uint64_t)identify[61] << 16);
            }
        }

        geo.total_bytes = geo.total_sectors * geo.sector_size;
        return geo;
    }


    uint64_t detect_size_simple() {
        char test_buffer[512];


        for (uint64_t test_sector = 1000; test_sector < (1ULL << 40); test_sector *= 2) {
            if (!read_sector_test(test_sector, test_buffer)) {
                return test_sector / 2;
            }
        }

        return 0;
    }

private:
    bool read_sector_test(uint64_t lba, char* buffer) {

        wait_bsy();

        outb(base_port + ATA_SECTOR_COUNT, 1);
        outb(base_port + ATA_LBA_LOW, lba & 0xFF);
        outb(base_port + ATA_LBA_MID, (lba >> 8) & 0xFF);
        outb(base_port + ATA_LBA_HIGH, (lba >> 16) & 0xFF);
        outb(base_port + ATA_DRIVE_HEAD, 0xE0 | ((lba >> 24) & 0x0F));
        outb(base_port + ATA_COMMAND, 0x20);


        for (int i = 0; i < 1000; i++) {
            uint8_t status = inb(base_port + ATA_STATUS);
            if (!(status & 0x80)) {
                if (status & 0x08) {

                    for (int j = 0; j < 256; j++) {
                        *((uint16_t*)buffer + j) = inw(base_port + ATA_DATA);
                    }
                    return true;
                }
                if (status & 0x01) {
                    return false;
                }
            }
        }
        return false;
    }

public:
    uint16_t base_port;
    bool lba48_supported;
    DiskGeometry geometry;

    enum {
        ATA_DATA         = 0,
        ATA_ERROR        = 1,
        ATA_FEATURES     = 1,
        ATA_SECTOR_COUNT = 2,
        ATA_LBA_LOW      = 3,
        ATA_LBA_MID      = 4,
        ATA_LBA_HIGH     = 5,
        ATA_DRIVE_HEAD   = 6,
        ATA_STATUS       = 7,
        ATA_COMMAND      = 7
    };

    void wait_bsy() const {
        while (inb(base_port + ATA_STATUS) & 0x80) {}
    }

    void wait_drq() const {
        while (!(inb(base_port + ATA_STATUS) & 0x08)) {}
    }

    managed<char> read(uint64_t lba, uint32_t count) {
        managed<char> b;
        b.setsize(count * 512);
        read(lba, count, b.data());
        return b;
    }

    void read(uint64_t lba, uint32_t count, char* buffer) const {
        wait_bsy();

        outb(base_port + ATA_SECTOR_COUNT, count & 0xFF);
        outb(base_port + ATA_LBA_LOW, lba & 0xFF);
        outb(base_port + ATA_LBA_MID, (lba >> 8) & 0xFF);
        outb(base_port + ATA_LBA_HIGH, (lba >> 16) & 0xFF);
        outb(base_port + ATA_DRIVE_HEAD, 0xE0 | ((lba >> 24) & 0x0F));

        outb(base_port + ATA_COMMAND, 0x20);

        for (uint32_t sector = 0; sector < count; sector++) {
            wait_bsy();
            wait_drq();

            uint16_t temp_buffer[256];
            for (int i = 0; i < 256; i++) {
                temp_buffer[i] = inw(base_port + ATA_DATA);
            }

            memcpy(buffer + (sector * 512), temp_buffer, 512);
        }
    }

    bool write(uint64_t lba, const char* data) const {
        uint32_t count = 1;
        const char* buffer = data;

        wait_bsy();

        outb(base_port + ATA_SECTOR_COUNT, count & 0xFF);
        outb(base_port + ATA_LBA_LOW, lba & 0xFF);
        outb(base_port + ATA_LBA_MID, (lba >> 8) & 0xFF);
        outb(base_port + ATA_LBA_HIGH, (lba >> 16) & 0xFF);
        outb(base_port + ATA_DRIVE_HEAD, 0xE0 | ((lba >> 24) & 0x0F));

        outb(base_port + ATA_COMMAND, 0x30);

        for (uint32_t sector = 0; sector < count; sector++) {
            wait_bsy();
            wait_drq();

            const uint16_t* sector_buffer = (const uint16_t*)(buffer + (sector * 512));
            for (int i = 0; i < 256; i++) {
                outw(base_port + ATA_DATA, sector_buffer[i]);
            }
        }

        return true;
    }


    uint64_t get_total_sectors() const { return geometry.total_sectors; }
    uint32_t get_sector_size() const { return geometry.sector_size; }
    uint64_t get_total_bytes() const { return geometry.total_bytes; }
    bool is_lba48_supported() const { return geometry.lba48_supported; }
};
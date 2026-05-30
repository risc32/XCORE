#pragma once

extern "C" void middle();

void _start() {
    middle();
}

#define INTEL(...) asm volatile ("\n.intel_syntax noprefix");asm volatile (__VA_ARGS__ ); asm volatile(".att_syntax prefix")

typedef unsigned long long uint64_t;
typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;

static inline uint8_t inb(uint16_t port) {
    uint8_t result;
    asm volatile ("inb %w1, %b0"
            : "=a" (result)
            : "Nd" (port));
    return result;
}

static inline void outb(uint16_t port, uint8_t data) {
    asm volatile ("outb %b0, %w1"
            :
            : "a" (data), "Nd" (port));
}

static inline uint16_t inw(uint16_t port) {
    uint16_t result;
    asm volatile ("inw %w1, %w0"
            : "=a" (result)
            : "Nd" (port));
    return result;
}

static inline void outw(uint16_t port, uint16_t data) {
    asm volatile ("outw %w0, %w1"
            :
            : "a" (data), "Nd" (port));
}

#include "types/constant.cpp"

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

void wait_bsy() {
    while (inb(0x1F0 + ATA_STATUS) & 0x80) {}
}

void wait_drq() {
    while (!(inb(0x1F0 + ATA_STATUS) & 0x08)) {}
}

void read(uint64_t lba, uint32_t count, char* buffer) {
    wait_bsy();

    outb(0x1F0 + ATA_SECTOR_COUNT, count & 0xFF);
    outb(0x1F0 + ATA_LBA_LOW, lba & 0xFF);
    outb(0x1F0 + ATA_LBA_MID, (lba >> 8) & 0xFF);
    outb(0x1F0 + ATA_LBA_HIGH, (lba >> 16) & 0xFF);
    outb(0x1F0 + ATA_DRIVE_HEAD, 0xE0 | ((lba >> 24) & 0x0F));

    outb(0x1F0 + ATA_COMMAND, 0x20);

    for (uint32_t sector = 0; sector < count; sector++) {
        wait_bsy();
        wait_drq();

        uint16_t temp_buffer[256];
        for (int i = 0; i < 256; i++) {
            temp_buffer[i] = inw(0x1F0 + ATA_DATA);
        }

        __builtin_memcpy(buffer + (sector * 512), temp_buffer, 512);
    }
}

extern "C" void middle() {

    outb(0x3D4, 0x0A);
    outb(0x3D5, 0x20);

    uint64_t kernel_address = 0;

    int sectors = 2048;

    for (int i = 0; i < sectors / 256; i++) {
        read(39 + i * 256, 256, (char*)KERNELADDR + i * 512 * 256);
    }

    INTEL("jmp " STRKDRR);

}
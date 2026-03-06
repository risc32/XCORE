#pragma once

#include "../disk/disk.cpp"
#include "storage.cpp"
#include "../utils/inited.cpp"

struct result {
    inode ind;
    uint64_t sector;
};

struct Path {
    managed<string> parents;
    string endname;

    Path(string path) {
        auto s = split(path, '/');
        endname = s.back();
        s.pop_back();
        parents = s;
    }
};

struct dispatcher {
    static super_block superblock;
    static uint64_t csector;
    static bitmap cbitmap;
    static bool ismain;
    static bool changed;

    static void init() {
        char sb[512]{};
        disk::read(0, 1, sb);
        if (sb[510] == 0x55 && (unsigned char)sb[511] == 0xAA) {
            ismain = true;
        }
    }

    static void initsb() {
        disk::read(1, 1, superblock.data);
    }

    static void initbitmap() {
        csector = superblock.bitmap;
        changed = false;
        disk::read(csector, 1, cbitmap.data);
        
        char bp[512] = {};
        for (int i = 0; i < superblock.bsize; i++) {
            disk::write(superblock.bitmap + i, bp);
        }
        for (int i = 0; i < superblock.bsize + 2; i++) {
            set(i, true);
        }
        for (int i = 0; i < 1024; i++) {
            set(i + KERNEL, true);
        }
    }

    static uint64_t secpos(uint64_t lba) {
        return superblock.bitmap + (lba / (512 * 8));
    }

    static int bitpos(uint64_t lba) {
        return lba % (512 * 8);
    }

    static void set(uint64_t lba, bool used, bool autoc = true) {
        uint64_t sector = secpos(lba);
        int bit = bitpos(lba);

        if (sector != csector && changed) {
            disk::write(csector, cbitmap.data);
        }

        if (sector != csector) {
            disk::read(sector, 1, cbitmap.data);
            csector = sector;
            changed = false;
        }

        set_bit(cbitmap.data, bit, used);

        if (autoc) {
            changed = true;
        } else {
            disk::write(sector, cbitmap.data);
            changed = false;
        }
    }

    static bool get(uint64_t lba) {
        uint64_t sector = secpos(lba);

        if (sector != csector) {
            if (changed) {
                disk::write(csector, cbitmap.data);
            }
            disk::read(sector, 1, cbitmap.data);
            csector = sector;
            changed = false;
        }

        int bit = bitpos(lba);
        return get_bit(cbitmap.data, bit);
    }

    static void setcache(uint64_t sector = 0) {
        if (changed) {
            disk::write(csector, cbitmap.data);
            changed = false;
        }
        if (sector == 0) sector = csector;
        if (sector == csector) return;
        csector = sector;
        disk::read(csector, 1, cbitmap.data);
    }

    static uint64_t getfree() {
        for (int i = 0; i < superblock.total_blocks; ++i) {
            if (!get(i)) {
                set(i, true);
                return i;
            }
        }
        panic("Out of hardware memory");
    }

    static indirect getblock(uint64_t size) {

        int fco = 0;
        uint64_t start = 0;

        for (int i = 0; i < superblock.total_blocks; ++i) {
            if (!get(i)) {
                if (fco == 0) start = i;
                fco++;
                if (fco == size) {
                    for (uint64_t j = start; j < start + size; j++) {
                        set(j, true);
                    }
                    return {size, start};
                }
            } else {
                fco = 0;
            }
        }
        return {};
    }

    static void setblock(indirect ind, bool t) {
        for (int i = 0; i < ind.blocks; ++i) {
            set(i+ind.start, t);
        }
    }

    static uint64_t getfreecount() {
        int c = 0;
        for (int i = 0; i < superblock.total_blocks; ++i) {
            if (!get(i)) {
                c++;
            }
        }
        return c;
    }

    static super_block crblock() {
        uint64_t bms = ((disk::driver.geometry.total_sectors + 7) / 8 + 511) / 512;
        super_block sp = {};
        memcpy(sp.magic, "XCFS", 5);
        sp.version = VERSION;
        sp.kerneladdr = 39;
        sp.block_size = BLOCK_SIZE;
        sp.total_blocks = disk::driver.geometry.total_sectors;
        sp.bitmap = 2;
        sp.bsize = bms;
        return sp;
    }

    static uint64_t add(inode v) {
        uint64_t addr = getfree();

        if (v.nd.depth == 0) {
            v.nd.depth = 1;
        }
        v.index = addr;
        disk::write(addr, v.data);
        return addr;
    }

    static result add_result(inode v) {
        return {v, add(v)};
    }
};

super_block dispatcher::superblock;
bool dispatcher::ismain = false;
uint64_t dispatcher::csector = 0;
bitmap dispatcher::cbitmap = {};
bool dispatcher::changed = false;
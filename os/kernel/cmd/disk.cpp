#pragma once

#include "../types/scalar.cpp"
#include "../cpu/ports.cpp"
#include "../managed/managed.cpp"
#include "../stream/stream.cpp"
#include "../utils/inited.cpp"
#include "../disk/disk.cpp"
#include "cmd.cpp"

int disk(const managed<string>& args) {
    KernelOut kout;


    CHECK(1)
    string com = args[0];

    //kout << (args[0] == "readsec") << ":" << args[1].size() << endl;

    if (com == "info") {
        kout << "XCore File System v1.0 XCFS" << endl;
        kout << "BASE PORT: " << hex << disk::driver.base_port << endl;
        kout << "DISK SIZE: " << disk::driver.geometry.total_bytes << endl;
        kout << "SECTOR SIZE: " << dec << disk::driver.geometry.sector_size << endl;
        kout << "TOTAL SECTORS: " << disk::driver.geometry.total_sectors << endl;
        kout << "LBA48: " << disk::driver.lba48_supported << endl;
    }
    else if (com == "readsec") {
        CHECK(2)
        auto res = disk::driver.read(to_int(args[1]), 1);
        int c = 0;
        if (res.empty()) {
            kout << "DISK ERROR" << endl;
            return 1;
        }
        for (char r : res) {
            kout << r;
            if (c++%64 == 63) {
                kout << endl;
            }
        }
    } else if (com == "writesec") {
        CHECK(2)
        char *block = (char*)((memory_block*)parseaddr(args[2])+1);

        auto res = disk::driver.write(to_int(args[1]), block);
        int c = 0;
        if (!res) {
            kout << "DISK ERROR" << endl;
            return 1;
        }
        //kout << "SUCCESSFUL" << endl;

    } else {
        kout << "UNKNOWN COMMAND: " << '"' << com << '"' << endl;
        return 1;
    }

    return 0;
}
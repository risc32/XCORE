#pragma once

#include "../disk/disk.cpp"
#include "storage.cpp"
#include "../stream/stream.cpp"
#include "bitmap.cpp"
#include "../tree/tree.cpp"

struct filesystem {
    static bool mounted;
    static super_block& superblock;
    static tree bst;

    static void init() {
        bst.sector = superblock.root;
        mounted = false;
        bst = {};
    }

    static void mount() {
        KernelOut kout;
        KernelIn kin;

        string ans = "!yes";
        dispatcher::init();


        //kout << bg();
        kout.clear();
        kout << WHITE << "XCore";
        kout << reset << " File System v2.1 XCFS " << (dispatcher::ismain ? "[boot disk]" : "") << endl << endl;












        dispatcher::initsb();
        if (memcmp(dispatcher::superblock.magic, "XCFS", 4) != 0) {
            kout << "The current file system on this disk is not supported." << endl << RED << "Format disk?" << LIGHT_RED << " This action cannot be undone." << WHITE << " [yes/no]" << endl << CYAN;
            while (ans != "yes" && ans != "no") {
                kout << " > ";
                ans = _kcons::console.readLine();
            }
            if (ans == "yes") {

                format();
            }
        } else {
            kout << GREEN << "The kernel is ready to run." << LIGHT_GREEN << " Disk is mounted" << endl;
        }
        kout << CYAN << "Press space to enter the kernel" << endl;
        mounted = true;
        waitChar(' ');
    }


    static void format();
};
bool filesystem::mounted = false;
super_block& filesystem::superblock = dispatcher::superblock;
tree filesystem::bst = {};

#include "format.cpp"

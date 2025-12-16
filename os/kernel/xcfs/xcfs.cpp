#pragma once

#include "../disk/disk.cpp"
#include "storage.cpp"
#include "../stream/stream.cpp"
#include "bitmap.cpp"
#include "../tree/tree.cpp"
#include "../utils/inited.cpp"

struct filesystem {
    static bool mounted;
    static super_block& superblock;
    static tree bst;

    static void init() {
        s0::put("void filesystem::init() KERNEL 0x20000 .text\n");

        bst.sector = superblock.root;
        mounted = false;
        bst = {};
    }

    static void mount() {
        s0::put("void filesystem::mount() KERNEL 0x20000 .text\n");

        KernelOut kout;
        KernelIn kin;

        string ans = "";
        dispatcher::init();


        //kout << bg();
        kout.clear();
        kout << WHITE << "XCore";
        kout << reset << " File System v2.1 XCFS " << (dispatcher::ismain ? "[boot disk]" : "") << endl << endl;
#define ydiskans
#ifdef ydiskans
        ans = "yes";
#endif

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
#ifndef ydiskans
        waitChar(' ');
#endif
    }

    static inode create_file(string name) {
        auto spl = split(name, '/');
        if (bst.search(spl[spl.size()-2]).type != it_file) panic("There cannot be other elements in the file");
        inode ind(spl[spl.size()-1], it_file, 644, bst.lastaddr);
        memcpy(ind.fragments, fragment::hardwarealloc(1).data(), sizeof ind.fragments);\
        bst.insert(ind);
        return ind;
    }

    static void format();
};
bool filesystem::mounted = false;
super_block& filesystem::superblock = dispatcher::superblock;
tree filesystem::bst = {};

#include "format.cpp"

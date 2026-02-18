#pragma once

#include "../disk/disk.cpp"
#include "storage.cpp"
#include "../stream/stream.cpp"
#include "bitmap.cpp"
#include "../tree/tree.cpp"
#include "../utils/inited.cpp"
#include "../crypto/uuid.cpp"

struct Directory;
struct result;

struct filesystem {
    static bool mounted;
    static super_block& superblock;
    static Directory root;

    static void init();

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

    static Directory getdir(string path);
    static inode getfile(string path);
    static bool exists(string path);

    static Directory create_dir(string path);

    static void format();
};

#include "directory.cpp"
#include "file.cpp"


void filesystem::init() {
    s0::put("void filesystem::init() KERNEL 0x20000 .text\n");

    root = Directory("/");
    mounted = false;
}

Directory filesystem::getdir(string path) {
    auto p = split(path, '/');
    auto tec = root;

    for (auto i : p) {
        tec = Directory{tec.find(i).ind};
    }
    return tec;
}

inode filesystem::getfile(string path) {
    auto p = split(path, '/');
    auto tec = root;
    auto fname = p[p.size() - 1];
    p.remove(p.size()-1);

    for (auto i : p) {
        tec = Directory{tec.find(i).ind};
    }
    return tec.find(fname).ind;
}

bool filesystem::exists(string path) {
    return getfile(path).valid();
}

Directory filesystem::create_dir(string path) {
    return Directory::create(path);
}

bool filesystem::mounted = false;
super_block& filesystem::superblock = dispatcher::superblock;
Directory filesystem::root = {"/"};

#include "format.cpp"
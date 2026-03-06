#pragma once

#include "../disk/disk.cpp"
#include "storage.cpp"
#include "../stream/stream.cpp"
#include "bitmap.cpp"
#include "../tree/tree.cpp"
#include "../utils/inited.cpp"
#include "../crypto/uuid.cpp"
#include "../debug/inited.cpp"

struct Directory;
struct result;

struct filesystem {
    static bool mounted;
    static super_block& superblock;
    static Directory root;

    static void init();
    static void rootinit();

    static result getdir(managed<string> path);

    static void mount() {
        s0::put("void filesystem::mount() KERNEL 0x20000 .text\n");

        KernelOut kout;
        KernelIn kin;

        string ans = "";
        dispatcher::init();



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
            dispatcher::initbitmap();
            kout << GREEN << "The kernel is ready to run." << LIGHT_GREEN << " Disk is mounted" << endl;
        }
        kout << CYAN << "Press space to enter the kernel" << endl;
        mounted = true;
        if (memcmp(dispatcher::superblock.magic, "XCFS", 4) == 0) {
            rootinit();
        }
#ifndef ydiskans
        waitChar(' ');
#endif
    }

    static result getdir(string path);
    static result getfile(string path);
    static bool exists(string path);

    static Directory create_dir(string path);

    static void format();
};

#include "directory.cpp"
#include "file.cpp"
#include "format.cpp"

void filesystem::init() {
    s0::put("void filesystem::init() KERNEL 0x20000 .text\n");

    mounted = false;
}

void filesystem::rootinit() {
    result rootres = {dispatcher::superblock.root ? dispatcher::superblock.root : 0, 0};
    if (rootres.ind.index == 0) {
        root = Directory::allocdir();
        dispatcher::superblock.root = root.linked.sector;
        disk::write(1, dispatcher::superblock.data);
    } else {
        disk::read(dispatcher::superblock.root, 1, rootres.ind.data);
        rootres.sector = dispatcher::superblock.root;
        root = Directory{rootres};
    }
}

result filesystem::getdir(managed<string> path) {
    auto tec = root;

    for (auto i : path) {
        tec = Directory{tec.find(i)};
    }
    return tec.getres();
}

result filesystem::getdir(string path) {
    auto p = split(path, '/');
    auto tec = root;

    for (auto i : p) {
        tec = Directory{tec.find(i)};
    }
    return tec.getres();
}

result filesystem::getfile(string path) {
    auto p = split(path, '/');
    auto tec = root;
    auto fname = p[p.size() - 1];
    p.remove(p.size()-1);

    for (auto i : p) {
        tec = Directory{tec.find(i)};
    }
    return tec.find(fname);
}

bool filesystem::exists(string path) {
    return getfile(path).ind.valid();
}

Directory filesystem::create_dir(string path) {
    return Directory::create(path);
}

bool filesystem::mounted = false;
super_block& filesystem::superblock = dispatcher::superblock;
Directory filesystem::root = Directory{""};


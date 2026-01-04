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

    static inode getfpath(string path) {
        auto spl = split(path, '/');
        inode parent;
        string fname = spl.back();
        spl.pop_back();
        uint64_t lsec = bst.sector;
        for (auto& s : spl) {
            if (s == "") panic("Invalid path");
            parent = bst.search(s, lsec);
            if (parent.type == it_none) panic("Path not found");
        }

        inode f = bst.search(fname, lsec);
        if (f.type == it_none) panic("Path not found");
        return f;
    }

    static inode create_file(string name) {
        auto spl = split(name, '/');
        inode parent;
        string fname = spl.back();
        spl.pop_back();
        uint64_t lsec = bst.sector;
        for (auto& s : spl) {
            if (s == "") panic("Invalid path");
            parent = bst.search(s, lsec);
            if (parent.type == it_none) panic("Path not found");
        }
        if (parent.type != it_file) panic("There cannot be other elements in the file");
        inode ind(fname, it_file, 644, lsec);
        memcpy(ind.fragments, fragment::hardwarealloc(1).data(), sizeof ind.fragments);
        bst.insert(ind);
        return ind;
    }

    static inode create_dir(string name) {
        auto spl = split(name, '/');
        inode parent;
        string fname = spl.back();
        spl.pop_back();
        uint64_t lsec = bst.sector;
        for (auto& s : spl) {
            if (s == "") panic("Invalid path");
            parent = bst.search(s, lsec);
            if (parent.type == it_none) panic("Path not found");
        }
        if (parent.type != it_file) panic("There cannot be other elements in the file");
        inode ind(fname, it_file, 644, lsec);
        memcpy(ind.fragments, fragment::hardwarealloc(1).data(), sizeof ind.fragments);
        bst.insert(ind);
        return ind;
    }

    static inode open_file(string name) {
        auto spl = split(name, '/');
        inode parent;
        string fname = spl.back();
        spl.pop_back();
        uint64_t lsec = bst.sector;
        for (auto& s : spl) {
            if (s == "") panic("Invalid path");
            parent = bst.search(s, lsec);
            if (parent.type == it_none) panic("Path not found");
        }
        return bst.search(fname, lsec);
    }

    static void remove(string name) {
        auto spl = split(name, '/');
        inode parent;
        string fname = spl.back();
        spl.pop_back();
        uint64_t lsec = bst.sector;
        for (auto& s : spl) {
            if (s == "") panic("Invalid path");
            parent = bst.search(s, lsec);
            if (parent.type == it_none) panic("Path not found");
        }
        //TODO: bst.remove(fname, lsec);
    }

    static void remove(inode ind) {
        //TODO: bst.remove(ind);
    }

    static managed<string> list(string name) {

    }

    static void format();
};
bool filesystem::mounted = false;
super_block& filesystem::superblock = dispatcher::superblock;
tree filesystem::bst = {};

#include "format.cpp"

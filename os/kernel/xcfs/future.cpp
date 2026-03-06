#pragma once

#include "../disk/disk.cpp"
#include "storage.cpp"
#include "../stream/stream.cpp"
#include "bitmap.cpp"
#include "../tree/tree.cpp"
#include "../utils/inited.cpp"
#include "../crypto/uuid.cpp"

struct result {
    inode ind;
    uint64_t sector;
};

enum OpenMode {
    read = 1 << 0,
    write = 1 << 1,
    append = 1 << 2
};



struct futurefilesystem {
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

    static result getfpath(string path) {

        auto spl = split(path, '/');
        inode parent;
        string fname = spl.back();
        spl.pop_back();
        uint64_t lsec = bst.sector;
        for (auto& s : spl) {
            if (s == "") panic("Invalid path");
            parent = bst.search(s, lsec);
            if (parent.type != it_dir) panic((string(parent.name) + " is not a directory"_s).c_str());
            if (parent.type == it_none) panic("Path not found");
        }

        inode f = bst.search(fname, lsec);
        return {f, lsec};
    }

    static string getfname(string path) {
        return split(path, '/').back();
    }

    static bool exist(string path) {
        auto spl = split(path, '/');
        inode parent;
        string fname = spl.back();
        spl.pop_back();
        uint64_t lsec = bst.sector;
        for (auto& s : spl) {
            if (s == "") return false;
            parent = bst.search(s, lsec);
            if (parent.type != it_dir) return false;
            if (parent.type == it_none) return false;
        }

        inode f = bst.search(fname, lsec);
        return f.type != it_none;
    }

    static string checkname(string name) {
        if (name.size() > 80) panic("Name too long");
        if (name == "") panic("Invalid name");
        for (auto c : name) {
            if (c == '/' || c == 0) panic("Invalid name");
        }
        return name;
    }

    static inode create_file(string name) {
        result parent = getfpath(name);
        if (parent.ind.type != it_none) panic("A file or directory with that name already exists");
        inode ind(checkname(getfname(name)), it_file, 644, parent.sector);
        memcpy(ind.fragments, fragment::hardwarealloc(1).data(), sizeof ind.fragments);
        bst.insert(ind);

        return ind;
    }

    static inode create_dir(string path);

    static void remove_dir(string path);

    static managed<string> list_dir(string path);

    static bool is_dir_empty(string path);

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

    }

    static void remove(inode ind) {

    }

    static managed<string> list(string name) {

    }

    static void write_file(string name, string data, uint64_t pos) {
        auto f = getfpath(name).ind;
        if (f.type != it_file) panic("Target is not a file");
        if (pos == -1) pos = f.getsize();
        fragment::write(f.fragments, pos, data);
    }

    static void read_file(string name, uint64_t pos, uint64_t size) {
        auto f = getfpath(name).ind;
        if (f.type != it_file) panic("Target is not a file");
        if (pos == -1) pos = f.getsize();
        fragment::read(f.fragments, pos, size);
    }

    static void format();
};

#include "directory.cpp"

inode filesystem::create_dir(string path) {
    return Directory::create(path);
}

void filesystem::remove_dir(string path) {
    auto target = getfpath(path);

    if (target.ind.type != it_dir) {
        panic("Not a directory");
    }


    if (!Directory::is_empty(target.ind)) {
        panic("Directory not empty");
    }


    inode parent;
    disk::read(target.sector, 1, parent.data);


    Directory::remove_entry(parent, getfname(path));


    for (auto& frag : target.ind.fragments) {
        if (frag.blocks > 0) {
            dispatcher::setblock(frag, false);
        }
    }


    dispatcher::set(target.ind.index, false);


    disk::write(parent.index, parent.data);
}

managed<string> filesystem::list_dir(string path) {
    auto target = getfpath(path);

    if (target.ind.type != it_dir) {
        panic("Not a directory");
    }

    return Directory::list(target.ind);
}

bool filesystem::is_dir_empty(string path) {
    auto target = getfpath(path);
    return Directory::is_empty(target.ind);
}

bool filesystem::mounted = false;
super_block& filesystem::superblock = dispatcher::superblock;
tree filesystem::bst = {};

#include "format.cpp"


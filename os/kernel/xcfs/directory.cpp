#pragma once

#include "storage.cpp"
#include "bitmap.cpp"
#include "../utils/inited.cpp"
#include "../debug/inited.cpp"
#include "../memory/memory.cpp"
#include "xcfs.cpp"
#include "../crypto/crypto.cpp"

#define ENAMESZ 232
#define EDSCOUNT 16

#define CHECK(path) {if (!check(path)) panic("Invalid name");}
#define CHECKF(path) {if (!fcheck(path)) panic("Invalid path");}

struct DirectoryEntry {
    __int128 hash;
    uint64_t ind;
    char name[ENAMESZ];

    bool compare(const string& other) const {
        return memcmp(name, other.data(), ENAMESZ) == 0;
    }
};

struct DirectoryStorage {
    DirectoryEntry entries[EDSCOUNT];
};

struct result;

struct Directory {
    inode dir;

    static Directory create(const string& path) {
        CHECKF(path)
        inode i = filesystem::getdir(path).dir;
        if (i.type != it_none) panic("A file or directory with that name already exists");
        __annotation()
        memcpy(i.fragments, fragment::hardwarealloc(1).data(), sizeof i.fragments);

        //filesystem::bst.insert(ind);
        return Directory(i);
    }

    Directory(inode _dir) : dir(_dir) {
        if (dir.type != it_dir) panic("The specified directory does not exist");
    }

    Directory(const string& path) {
        CHECKF(path)

        dir = filesystem::getdir(path).dir;
        if (dir.type != it_dir) panic("The specified directory does not exist");
    }

    DirectoryStorage get_storage(uint64_t pos) {
        return *(DirectoryStorage*)fragment::read(dir.fragments, pos * sizeof(DirectoryStorage), sizeof(DirectoryStorage)).data();
    }

    void set_storage(uint64_t pos, DirectoryStorage data) {
        fragment::write(dir.fragments, pos * sizeof(DirectoryStorage), bytestream(data));
    }

    DirectoryEntry* find_entry(const string& entry) {
        if (entry.size() > ENAMESZ) return nullptr;
        __int128 entryhash = hash::_highprec1(entry.c_str());
        int pos = 0;
        while (true) {
            for (auto& i : get_storage(pos++).entries) {
                if (i.hash == entryhash and i.compare(entry)) return &i;
            }
            if (pos >= dir.getsize() / EDSCOUNT) break;
        }
        return nullptr;
    }

    result find(const string& entry);

    managed<string> list() {
        managed<string> res;
        int pos = 0;
        while (true) {
            for (auto& i : get_storage(pos++).entries) {
                res.push_back(i.name);
            }
            if (pos >= dir.getsize() / EDSCOUNT) break;
        }
        return res;
    }

    bool has_entry(const string& entry) {
        return find_entry(entry) != nullptr;
    }

    static bool check(const string& entry) {

        for (char i : entry) {
            if (i == '/' or i == '\n' or i == 0) return false;

        }
        if (entry == ".") return false;
        if (entry == "..") return false;
        if (entry.size() > ENAMESZ) return false;

        return true;
    }

    static bool fcheck(const string& entry) {

        for (char i : entry) {
            if (i == '\n' or i == 0) return false;
        }
        if (entry == ".") return false;
        if (entry == "..") return false;
        if (entry.size() > ENAMESZ) return false;

        return true;
    }


    void add_entry(const string& entry) {
        CHECK(entry)
        if (!has_entry(entry)) {
            DirectoryEntry ent{hash::_highprec1(entry.c_str())};
            memcpy(ent.name, entry.data(), entry.size());
            fragment::write(dir.fragments, -1, bytestream(ent));
        }
    }
};

struct result {
    inode ind;
    uint64_t sector;
};

result none = {};

result Directory::find(const string& entry) {
    DirectoryEntry* found = find_entry(entry);
    if (found == nullptr) {
        return none;
    }
    inode ind;
    disk::read(found->ind, 1, ind.data);

    return {
        ind,
        found->ind
    };
}
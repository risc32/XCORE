#pragma once
//#define stage2
#include "../types/types.cpp"

#ifndef stage2
#include "../cpu/cpu.cpp"
#include "../disk/disk.cpp"

#define VERSION 1
#define BLOCK_SIZE 512
#define FRAGCOUNT 26

struct __attribute__((packed)) node {
    uint64_t parent;

    uint64_t a;
    uint64_t b;

    int depth;

    int size;
} ;

struct dir_entry {
    uint64_t inode;
    uint16_t next_offset;
    uint8_t name_len;
    uint8_t type;
    char name[256];
};
#endif

union super_block {
    char data[512];
    struct {
        char magic[5];
        uint64_t version;

        uint64_t kerneladdr;

        uint64_t block_size;
        uint64_t total_blocks;
        uint64_t free_inodes;

        uint64_t root;
        uint64_t bitmap;
        uint64_t bsize;

        uint64_t last_mount;
        uint64_t last_write;
    };

    super_block() {
        memset(data, 0, sizeof(data));
    };
};

#ifndef stage2

struct indirect {
    uint64_t blocks;
    uint64_t start;
};

enum inode_type {
    it_none,
    it_file,
    it_dir,
    it_symbol
};

union inode {
    char data[512];
    struct {
        node nd;

        uint64_t index;
        indirect fragments[FRAGCOUNT];

        inode_type type;
        uint16_t permissions;

        uint64_t created;
        uint64_t modified;
        uint64_t size;

        uint64_t link_count;
        uint64_t uid;
        uint64_t gid;
    };

    void write() {

    }

    uint64_t getsize() const {
        return size;
    }

    bool valid() {
        return type != it_none;
    }

    inode (inode_type type,
           uint64_t permissions = 0644, uint64_t parent = 0) {


        this->type = type;
        this->permissions = permissions;
        link_count = 1;
        uid = 0;
        gid = 0;


        //memcpy(name, filename.data(), (filename).size());
        //name[sizeof(name) - 1] = '\0';

        created = 0;
        modified = 0;
        size = 0;

        nd.parent = parent;
        nd.a = 0;
        nd.b = 0;
        nd.depth = 1;

        for (auto & fragment : fragments) {
            fragment.blocks = 0;
            fragment.start = 0;
        }
    }

    inode() {

    }
};

struct linkednode {
    inode ind;
    uint64_t sector;

    void save() {
        disk::write(sector, ind.data);
    }
};

union inode_table {
    char data[512];
    struct {
        uint64_t fragments[60];
        uint64_t next;
    };
};

struct bitmap {
    char data[512];
};

#endif
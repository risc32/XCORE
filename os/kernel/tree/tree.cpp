#pragma once

#include "../cpu/cpu.cpp"
#include "../disk/disk.cpp"
#include "../xcfs/storage.cpp"
#include "../xcfs/bitmap.cpp"
#include "../stream/stream.cpp"
#include "fragment.cpp"

struct path {
    struct changeable {
        uint64_t* data;
        int index;

        operator bool () {
            return get_bit((char*)&data, index);
        }

        changeable& operator=(bool k) {
            set_bit((char*)&data, index, k);
            return *this;
        }
    };
    uint64_t data;
    int size = 0;

    bool operator[](int index) {
        return changeable{.data = &data, .index = index};
    }
    path operator+(bool k) const {
        path p = *this;
        set_bit((char*)&p.data, p.size++, k);
        return p;
    }
    uint64_t* begin() {
        return &data;
    }
    uint64_t* end() {
        return &data+size;
    }
};

struct tree {
    uint64_t sector = 0;
    uint64_t lastaddr = 0;

    static inode getinode(uint64_t addr) {
        inode i{};
        disk::read(addr, 1, i.data);
        return i;
    }

    inode search(path path) {
        inode current = getinode(sector);
        lastaddr = sector;

        for (const auto &item: path) {
            uint64_t next_addr = item ? current.nd.b : current.nd.a;

            if (next_addr == 0) {
                inode empty{};
                lastaddr = 0;
                return empty;
            }

            current = getinode(next_addr);
            lastaddr = next_addr;
        }

        return current;
    }

    static int get_depth(uint64_t node_addr) {
        if (node_addr == 0) return 0;
        inode node = getinode(node_addr);
        return node.nd.depth;
    }

    static void update_depth(uint64_t node_addr) {
        if (node_addr == 0) return;

        inode node = getinode(node_addr);
        int left_depth = get_depth(node.nd.a);
        int right_depth = get_depth(node.nd.b);

        int new_depth = 1 + max(left_depth, right_depth);

        if (node.nd.depth != new_depth) {
            node.nd.depth = new_depth;
            write(node_addr, node);
            KernelOut() << "  Updated depth of " << node.name << " to " << new_depth << endl;
        }
    }

    static int balance_factor(uint64_t addr) {
        if (addr == 0) return 0;
        inode node = getinode(addr);
        return get_depth(node.nd.a) - get_depth(node.nd.b);
    }

    static uint64_t rotr(uint64_t addr) {
        if (addr == 0) return 0;

        inode node = getinode(addr);
        if (node.nd.a == 0) return addr;

        inode left_child = getinode(node.nd.a);

        uint64_t child_right = left_child.nd.b;
        left_child.nd.b = addr;
        node.nd.a = child_right;


        update_depth(addr);
        update_depth(left_child.index);

        write(addr, node);
        write(left_child.index, left_child);

        return left_child.index;
    }

    static uint64_t rotl(uint64_t addr) {
        if (addr == 0) return 0;

        inode node = getinode(addr);
        if (node.nd.b == 0) return addr;

        inode right_child = getinode(node.nd.b);

        uint64_t child_left = right_child.nd.a;
        right_child.nd.a = addr;
        node.nd.b = child_left;


        update_depth(addr);
        update_depth(right_child.index);

        write(addr, node);
        write(right_child.index, right_child);

        return right_child.index;
    }

    static uint64_t balance(uint64_t addr) {
        if (addr == 0) return 0;

        inode i = getinode(addr);
        int bf = balance_factor(addr);

        if (bf > 1) {
            if (balance_factor(i.nd.a) >= 0) {
                return rotr(addr);
            } else {
                i.nd.a = rotl(i.nd.a);
                write(addr, i);
                return rotr(addr);
            }
        } else if (bf < -1) {
            if (balance_factor(i.nd.b) <= 0) {
                return rotl(addr);
            } else {
                i.nd.b = rotr(i.nd.b);
                write(addr, i);
                return rotl(addr);
            }
        }
        return addr;
    }

    inode search(const string& name, uint64_t sec = 0) {
        if (sec == 0) sec = sector;
        inode tec = getinode(sec);
        int cmp = str_compare(tec.name, name.c_str());

        if (cmp == 0) {
            lastaddr = sec;
            return tec;
        }

        if (cmp > 0) {
            if (tec.nd.a == 0) {
                lastaddr = 0;
                return {};
            } else {
                return search(name, tec.nd.a);
            }
        } else {
            if (tec.nd.b == 0) {
                lastaddr = 0;
                return {};
            } else {
                return search(name, tec.nd.b);
            }
        }
    }

    static uint64_t add(inode v) {
        uint64_t addr = dispatcher::getfree();

        if (v.nd.depth == 0) {
            v.nd.depth = 1;
        }
        v.index = addr;
        disk::write(addr, v.data);
        return addr;
    }

    static void write(uint64_t addr, inode v) {
        v.index = addr;
        disk::write(addr, v.data);
    }

    bool initialize_filesystem(bool noroot = false) {
        if (sector != 0) return true;
        if (!noroot) {
            KernelOut() << endl << "noroot init" << endl;
            sector = dispatcher::superblock.root;
            return true;
        }
        KernelOut() << endl << "root init" << endl;

        inode root = inode("root", it_dir);
        sector = add(root);
        lastaddr = sector;
        dispatcher::superblock.root = lastaddr;
        disk::write(1, dispatcher::superblock.data);

        KernelOut() << endl << "Root address: " << lastaddr << endl;

        return true;
    }

    bool insert(inode value) {
        if (sector == 0) initialize_filesystem();

        int v = add(value);
        uint64_t current_addr = sector;
        inode current = getinode(current_addr);
        int s = 0;

        while (true) {
            int sz = strlen(current.name) == strlen(value.name)
                     ? strlen(value.name)
                     : sizeof(inode::name);
            int cmp = memcmp(current.name, value.name, sz);
            ++s;
            if (cmp == 0) {
                KernelOut() << s << '"' << current.name << '"' << endl;
                return false;
            }

            inode current_original;
            memcpy(current_original.data, current.data, sizeof(current.data));

            if (cmp > 0) {
                if (current.nd.a == 0) {
                    current.nd.a = v;
                    if (memcmp(current_original.data, current.data, sizeof(current.data)) != 0) {
                        write(current_addr, current);
                    }
                    return true;
                } else {
                    current_addr = current.nd.a;
                    current = getinode(current_addr);
                }
            } else {
                if (current.nd.b == 0) {
                    current.nd.b = v;
                    if (memcmp(current_original.data, current.data, sizeof(current.data)) != 0) {
                        write(current_addr, current);
                    }
                    return true;
                } else {
                    current_addr = current.nd.b;
                    current = getinode(current_addr);
                }
            }
        }

    }

    static int str_compare(const char* s1, const char* s2) {
        while (*s1 && *s2 && *s1 == *s2) {
            s1++;
            s2++;
        }
        return *s1 - *s2;
    }
};
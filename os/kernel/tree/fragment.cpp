#pragma once

#include "../xcfs/storage.cpp"
#include "../xcfs/bitmap.cpp"

struct fragment {
    // 1 - 512 / 1024 = 0.5  ; + fcount
    // 1 - 256 / 1024 = 0.75 ; - fcount

    // allocate disk space with the possibility of fragmentation
    static managed<indirect> hardwarealloc(size_t size) {
//        uint64_t scount = (512 + size) / 512;
        managed<indirect> res = {};
        uint64_t allc = 0;
        bool sc = true;

        for (int fragc = 1; fragc < 64; ++fragc) {
            res.clear();
            for (int i = 0; i <= fragc; ++i) {
                indirect ind;

                if (i == fragc) ind = dispatcher::getblock((max(size-allc, 0)));
                else ind = dispatcher::getblock((size/fragc));
                allc += size/fragc;
                //KernelOut() << size/fragc*i << endl;
                if (ind.blocks == 0) {
                    sc = false;
                    allc = 0;
                    dispatcher::setblock(ind, false);
                    break;
                }
                res.push_back(ind);
            }
            if (sc) {
                return res;
            }
            sc = true;
        }
        panic("Out of hardware memory");
    }

    static uint64_t getsize(managed<indirect> frag) {
        size_t size = 0;
        for (indirect ind : frag) {
            size += ind.blocks * 512;
        }
        return size;
    }

    static void write(managed<indirect> frag, const managed<char>& data) {
        const char* d = data.data();
        uint64_t size = getsize(frag);
        if (size < data.size()) frag += hardwarealloc(data.size() - size + 0);
        //if (size < data.size()) panic("Invalid fragmentation [frag.size < data.size]");
        for (indirect ind : frag) {
            for (int i = 0; i < ind.blocks; ++i) {
                disk::write(ind.start+i, d);
                d += 512;
            };
        }
    }

    static managed<char> read(managed<indirect> frag) {
        managed<char> res{getsize(frag)};
        res.setsize(res.capacity());
        uint64_t s = 0;

        for (indirect ind : frag) {
            KernelOut() << ind.start <<" : "<<ind.blocks<<endl;
            disk::read(ind.start, ind.blocks, res.data()+s);
            s += ind.blocks * 512;
        }

        return res;
    }
};
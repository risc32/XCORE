#pragma once

#include "../xcfs/storage.cpp"
#include "../xcfs/bitmap.cpp"

struct fragment {
    static managed<indirect> to_managed(indirect* frag) {
        managed<indirect> a = {};
        for (int i = 0; frag[i].blocks; i++) {
            a.push_back(frag[i]);
        }
        return a;
    }

    static void write(indirect* frag, uint64_t seek, const managed<char>& data) {
        write(to_managed(frag), seek, data);
    }


    static void write(managed<indirect> frag, uint64_t seek, const managed<char>& data) {
        const char* d = data.data();
        uint64_t remaining = data.size();
        uint64_t current_seek = seek;


        uint64_t total_size = getsize(frag);
        if (seek == -1) {
            current_seek = total_size;
        }
        if (current_seek + remaining > total_size) {
            uint64_t needed = (current_seek + remaining) - total_size;
            frag += hardwarealloc(needed);
        }


        for (indirect& ind : frag) {
            uint64_t block_size = ind.blocks * 512;

            if (current_seek >= block_size) {
                current_seek -= block_size;
                continue;
            }


            uint64_t start = current_seek / 512;
            uint64_t offset = current_seek % 512;
            uint64_t count = ind.blocks - start;
            uint64_t available = count * 512 - offset;

            if (remaining <= available) {

                uint64_t blocks_needed = (remaining + offset + 511) / 512;
                disk::write(ind.start + start, blocks_needed, d);
                return;
            } else {

                if (offset > 0) {

                    managed<char> temp(512);
                    disk::read(ind.start + start, 1, temp.data());
                    memcpy(temp.data() + offset, d, 512 - offset);
                    disk::write(ind.start + start, temp.data());
                    d += 512 - offset;
                    remaining -= 512 - offset;
                    start++;
                }


                if (start < ind.blocks) {
                    uint64_t full_blocks = min(remaining / 512, ind.blocks - start);
                    if (full_blocks > 0) {
                        disk::write(ind.start + start, full_blocks, d);
                        d += full_blocks * 512;
                        remaining -= full_blocks * 512;
                    }
                }
                current_seek = 0;
            }
        }
    }

    static managed<char> read(indirect* frag, uint64_t seek, uint64_t size) {
        return read(to_managed(frag), seek, size);
    }

    static managed<char> read(managed<indirect> frag, uint64_t seek, uint64_t size) {
        managed<char> res(size);
        char* buffer = res.data();
        uint64_t remaining = size;
        uint64_t current_seek = seek;
        uint64_t buffer_offset = 0;


        uint64_t total_size = getsize(frag);
        if (seek + size > total_size) {
            size = total_size - seek;
            remaining = size;
            res.setsize(size);
        }


        for (indirect& ind : frag) {
            uint64_t block_size = ind.blocks * 512;

            if (current_seek >= block_size) {
                current_seek -= block_size;
                continue;
            }


            uint64_t start_block = current_seek / 512;
            uint64_t block_offset = current_seek % 512;
            uint64_t blocks_available = ind.blocks - start_block;
            uint64_t bytes_available = blocks_available * 512 - block_offset;

            if (remaining <= bytes_available) {

                uint64_t blocks_needed = (remaining + block_offset + 511) / 512;
                disk::read(ind.start + start_block, blocks_needed, buffer + buffer_offset);

                if (block_offset > 0 || remaining % 512 != 0) {

                    memmove(buffer + buffer_offset,
                            buffer + buffer_offset + block_offset,
                            remaining);
                }
                return res;
            } else {

                if (block_offset > 0) {

                    managed<char> temp(512);
                    disk::read(ind.start + start_block, 1, temp.data());
                    uint64_t chunk = min(512 - block_offset, remaining);
                    memcpy(buffer + buffer_offset, temp.data() + block_offset, chunk);
                    buffer_offset += chunk;
                    remaining -= chunk;
                    start_block++;
                }


                if (start_block < ind.blocks && remaining > 0) {
                    uint64_t full_blocks = min(remaining / 512, ind.blocks - start_block);
                    if (full_blocks > 0) {
                        disk::read(ind.start + start_block, full_blocks,
                                  buffer + buffer_offset);
                        buffer_offset += full_blocks * 512;
                        remaining -= full_blocks * 512;
                    }
                }
                current_seek = 0;
            }
        }

        return res;
    }


    static void write_full(managed<indirect>& frag, const managed<char>& data) {
        write(frag, 0, data);
    }


    static managed<char> read_full(managed<indirect>& frag) {
        return read(frag, 0, getsize(frag));
    }


    static managed<indirect> hardwarealloc(size_t size) {
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

    static uint64_t getsize(managed<indirect>& frag) {
        size_t size = 0;
        for (indirect& ind : frag) {
            size += ind.blocks * 512;
        }
        return size;
    }
};
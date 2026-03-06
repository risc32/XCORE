#pragma once

#include "../xcfs/storage.cpp"
#include "../xcfs/bitmap.cpp"
#include "../debug/inited.cpp"

struct fragment {
    static managed<indirect> to_managed(indirect *frag) {
        managed<indirect> a = {};
        for (int i = 0; frag[i].blocks; i++) {
            a.push_back(frag[i]);
        }
        return a;
    }

    static void write(indirect *frag, uint64_t seek, const managed<char> &data, linkednode linked) {
        write(to_managed(frag), seek, data, linked);
    }

    static void write(managed<indirect> frag, int64_t seek, const managed<char> &data, linkednode linked) {
        if (data.size() == 0) return;


        if (seek == -1) {
            seek = getsize(frag);
        }


        if (seek < 0) {
            panic("Negative seek position after -1 expansion");
        }

        const char *d = data.data();
        uint64_t bytes_remaining = data.size();
        uint64_t data_pos = 0;
        uint64_t current_seek = seek;

        for (auto &indir: frag) {
            uint64_t block_size = indir.blocks * 512;


            if (current_seek >= block_size) {
                current_seek -= block_size;
                continue;
            }


            uint64_t block_offset = current_seek % 512;
            uint64_t start_block = current_seek / 512;
            uint64_t bytes_available = block_size - current_seek;


            uint64_t bytes_to_write = min(bytes_remaining, bytes_available);

            if (bytes_to_write > 0) {

                if (block_offset == 0 && bytes_to_write % 512 == 0) {

                    disk::write(indir.start + start_block, bytes_to_write / 512, d + data_pos);
                } else {

                    uint64_t written = 0;


                    if (block_offset > 0) {
                        char sector_buffer[512];
                        disk::read(indir.start + start_block, 1, sector_buffer);

                        uint64_t chunk = min(512 - block_offset, bytes_to_write);
                        memcpy(sector_buffer + block_offset, d + data_pos, chunk);
                        disk::write(indir.start + start_block, sector_buffer);

                        written += chunk;
                        start_block++;
                    }


                    while (bytes_to_write - written >= 512) {
                        disk::write(indir.start + start_block,
                                    d + data_pos + written);
                        written += 512;
                        start_block++;
                    }


                    if (bytes_to_write - written > 0) {
                        char sector_buffer[512];
                        disk::read(indir.start + start_block, 1, sector_buffer);

                        uint64_t chunk = bytes_to_write - written;
                        memcpy(sector_buffer, d + data_pos + written, chunk);
                        disk::write(indir.start + start_block, sector_buffer);
                    }
                }

                data_pos += bytes_to_write;
                bytes_remaining -= bytes_to_write;
                current_seek = 0;
            }

            if (bytes_remaining == 0) break;
        }
    }

    static managed<char> read(indirect *frag, uint64_t seek, uint64_t size) {
        return read(to_managed(frag), seek, size);
    }

    static managed<char> read(managed<indirect> frag, int64_t seek, uint64_t size) {
        if (size == 0) return managed<char>();

        uint64_t total_size = getsize(frag);


        if (seek == -1) {
            if (size > total_size) {
                seek = 0;
            } else {
                seek = total_size - size;
            }
        }


        if (seek < 0 || static_cast<uint64_t>(seek) >= total_size) {
            return managed<char>();
        }

        uint64_t u_seek = static_cast<uint64_t>(seek);


        if (u_seek + size > total_size) {
            size = total_size - u_seek;
        }

        managed<char> res(size);
        char *buffer = res.data();
        uint64_t bytes_remaining = size;
        uint64_t buffer_pos = 0;
        uint64_t current_seek = u_seek;

        for (auto &ind: frag) {
            uint64_t block_size = ind.blocks * 512;


            if (current_seek >= block_size) {
                current_seek -= block_size;
                continue;
            }


            uint64_t block_offset = current_seek % 512;
            uint64_t start_block = current_seek / 512;
            uint64_t bytes_available = block_size - current_seek;


            uint64_t bytes_to_read = min(bytes_remaining, bytes_available);

            if (bytes_to_read > 0) {
                if (block_offset == 0 && bytes_to_read % 512 == 0) {

                    disk::read(ind.start + start_block, bytes_to_read / 512,
                               buffer + buffer_pos);
                } else {

                    uint64_t read = 0;


                    if (block_offset > 0) {
                        char sector_buffer[512];
                        disk::read(ind.start + start_block, 1, sector_buffer);

                        uint64_t chunk = min(512 - block_offset, bytes_to_read);
                        memcpy(buffer + buffer_pos, sector_buffer + block_offset, chunk);

                        read += chunk;
                        start_block++;
                    }


                    if (bytes_to_read - read >= 512) {
                        uint64_t full_blocks = (bytes_to_read - read) / 512;
                        disk::read(ind.start + start_block, full_blocks,
                                   buffer + buffer_pos + read);
                        read += full_blocks * 512;
                        start_block += full_blocks;
                    }


                    if (bytes_to_read - read > 0) {
                        char sector_buffer[512];
                        disk::read(ind.start + start_block, 1, sector_buffer);

                        uint64_t chunk = bytes_to_read - read;
                        memcpy(buffer + buffer_pos + read, sector_buffer, chunk);
                    }
                }

                buffer_pos += bytes_to_read;
                bytes_remaining -= bytes_to_read;
                current_seek = 0;
            }

            if (bytes_remaining == 0) break;
        }

        return res;
    }

    static void write_full(managed<indirect> &frag, const managed<char> &data, linkednode linked) {
        write(frag, 0, data, linked);
    }

    static managed<char> read_full(managed<indirect> &frag) {
        return read(frag, 0, getsize(frag));
    }

    static managed<indirect> hardwarealloc(size_t size) {
        managed<indirect> res = {};

        for (int fragc = 1; fragc < FRAGCOUNT; ++fragc) {
            res.clear();
            size_t allocated = 0;
            size_t remaining = size;

            for (int i = 0; i < fragc; ++i) {
                size_t block_size;
                if (i == fragc - 1) {
                    block_size = remaining;
                } else {
                    block_size = size / fragc;
                    remaining -= block_size;
                }

                indirect ind = dispatcher::getblock(block_size);

                if (ind.blocks == 0) {
                    for (auto &block: res) {
                        dispatcher::setblock(block, false);
                    }
                    res.clear();
                    break;
                }

                res.push_back(ind);
                allocated += block_size;
            }

            if (allocated == size && res.size() == static_cast<size_t>(fragc)) {
                return res;
            }
        }

        panic("Out of hardware memory");
    }

    static uint64_t getsize(managed<indirect> &frag) {
        size_t size = 0;
        for (indirect &ind: frag) {
            size += ind.blocks * 512;
        }
        return size;
    }
};

#pragma once

#include "../types/scalar.cpp"
#include "../cpu/ports.cpp"
#include "../managed/managed.cpp"
#include "../stream/stream.cpp"
#include "../utils/inited.cpp"
#include "cmd.cpp"

int ram(const managed<string>& args) {
    KernelOut kout;

    CHECK(1)
    string com = args[0];

    if (com == "info") {
        auto blocks = getblocks();

        uint32_t usize = 0;
        for (memory_block* block : blocks) {
            usize += block->size * block->used;
        }
        kout << hex << "HEAP SIZE: " << HEAP_SIZE << endl;
        kout << hex << "HEAP ADDRESS: " << (int)memory::heap << endl;

        kout << "USED SPACE: " << usize << endl;
        kout << "FREE SPACE: " << HEAP_SIZE-usize << endl;
        kout << "COUNT OF BLOCKS: " << dec << blocks.size() << endl;
        kout << "TOTAL WAS ALLOCATED: " << memory::mbids << endl;

    } else if (com == "showfree") {
        auto blocks = getblocks();

        kout << "FREE MEMORY BLOCKS: " << endl << setw(10);
        blocktable(kout);

        for (memory_block* block : blocks) {
            if(!block->used) blockinfo(kout, block);
        }
    } else if (com == "get") {
        CHECK(2)

        memory_block* block = (memory_block*)parseaddr(args[1]);

        if(!block) {
            kout << "Block does not exist" << endl;
            return 1;
        }
        kout << "BLOCK INFO: " << endl << setw(10);
        oneblockinfo(block);
    } else if (com == "alloc") {
        CHECK(2)

        auto* block = ((memory_block*)allocate(to_int(args[1]))) - 1;
        if(!block) {
            kout << "Out of memory" << endl;
            return 1;
        }
        kout << "ALLOCATED BLOCK: " << endl;
        oneblockinfo(block);
    } else if (com == "realloc") {
        CHECK(3)

        auto* block = ((memory_block*)realloc(((memory_block*)parseaddr(to_int(args[1]))), to_int(args[2]))) - 1;
        if(!block) {
            kout << "Out of memory" << endl;
            return 1;
        }
        kout << "REALLOCATED BLOCK: " << endl;
        oneblockinfo(block);
    } else if (com == "free") {
        CHECK(2)

        memory_block *block = (memory_block *) parseaddr(args[1]);
        free(block);

        kout << "FREED BLOCK INFO: " << endl << setw(10);
        oneblockinfo(block);
    } else if (com == "place") {
        CHECK(2)

        char* start = ((char*)to_int(args[1], 16));
        memcpy(start, args[2].data(), args[2].size());

        //kout << "SUCCESSFUL" << endl;
        //oneblockinfo(block);
    } else if (com == "snap") {
        CHECK(2)
        char snap[512] = {};
        char* start = ((char*)to_int(args[1], 16)) - 256;
        memcpy(snap, start, 512);
        start = snap;

        for (char* i = start; i < start+512; ++i) {
            if ((i - start) % 64 == 0) {
                kout << " | ";
            }
            kout << *i;
            if ((i - start) % 64 == 63) {
                kout << endl;
            }
        }

    } else if (com == "getblock") {
        CHECK(2)
        oneblockinfo(((memory_block*)to_int(args[1]))-1);
    } else if (com == "browse") {
        CHECK(2)
        char c = 0;
        char* addr = (char*)to_int64(args[1], 16);
        do {
            if (c == 's') {
                addr += 64;
            } else if (c == 'w') {
                addr -= 64;
            } else if (c == 'S') {
                addr += 1024;
            } else if (c == 'W') {
                addr -= 1024;
            }
            kout.clear();
            kout << "Press SPACE to exit\nKey [lower W] - Change the address by 64\nKey [lower S] - Change the address by -64\nKey [upper W] - Change the address by 1024\nKey [upper S] - Change the address by -1024" << endl;
            for (char* i = addr; i < addr+1024; ++i) {
                if ((i - addr) % 64 == 0) {
                    kout << ' ' << swidth(to_wstring((int)i, 16), 6) << " > ";
                }
                kout << (*i != '\n' ? *i : ' ');
                if ((i - addr) % 64 == 63) {
                    kout << endl;
                }
            }
        } while((c = _kcons::console.readChar()) != ' ');
    } else {
        kout << "UNKNOWN COMMAND: " << '"' << com << '"' << endl;
        return 1;
    }

    return 0;
}
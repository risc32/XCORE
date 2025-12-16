#pragma once

#include "xcfs.cpp"

#define PROC(total, lbl, code) {kout << endl << "   "; \
Progress progress = {total, 30, lbl};code;progress.finish();own.increment();}


void filesystem::format() {
    KernelOut kout = {};
    kout << MAGENTA << "   ";
    Progress own(6, 30, "Formatting");
    PROC(1, "Preparation of structures", {
        dispatcher::superblock = dispatcher::crblock();
    });
    kout << YELLOW;
    PROC(1, "Superblock making", {
        disk::write(1, dispatcher::superblock.data);
    });

    PROC((int)dispatcher::superblock.bsize, "Bitmap making", {
        bitmap b = {};
        for (int i = 0; i < dispatcher::superblock.bsize; ++i) {
            disk::write(2, b.data);
            progress.increment();
        }
        disk::write(1, dispatcher::superblock.data);
    });

    dispatcher::initsb();
    dispatcher::initbitmap();


    PROC(1, "Root making", {
        if (!bst.initialize_filesystem(true)) panic("Error creating root directory");
    });

    kout << endl;
    kout << LIGHT_GREEN << "   Formatting complete" << endl;
}
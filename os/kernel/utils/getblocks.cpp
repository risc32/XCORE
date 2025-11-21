#pragma once

#include "../managed/managed.cpp"
#include "../memory/memory.cpp"

managed<memory_block*> getblocks() {  ///for stacktrace

    memory_block* f = memory::first->next;
    managed<memory_block*> res = {};
    while ((f = f)->used) {
        res.push_back((f = f->next));
    }
    return res;
}

memory_block* getbyid(int id) {
    memory_block* res = memory::first;
    while((res = res->next)->mbid != id) {
        if (res->next == nullptr) return nullptr;
    }
    return res;
}
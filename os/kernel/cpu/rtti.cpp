#include "../types/types.cpp"

extern "C" {

struct __vtable {
    size_t offset;
    void* type_info;
    void* functions[];
};

struct __type_info {
    const char* name;
};

void* __dynamic_cast(void* ptr, const __vtable* from, const __vtable* to) {
    if (from == to) return ptr;
    return nullptr;
}

}
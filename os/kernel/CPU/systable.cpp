#include "../types/types.cpp"

typedef struct {
    char signature[8];
    uint32_t revision;
    uint32_t header_size;

    void* boot_services;
} system_table_t;
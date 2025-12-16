
#include "../types/types.cpp"
#include "../memory/memory.cpp"



struct GDTDescriptor {
    uint16_t limit;
    uint64_t base;
} __attribute__((packed));


struct PageTableEntry {
    uint64_t value;

    void set_addr(uint64_t phys_addr) {
        value = (phys_addr & 0x000FFFFFFFFFF000) | (value & 0xFFF0000000000FFF);
    }

    void set_present(bool present) { value = (value & ~1ULL) | (present ? 1 : 0); }
    void set_writable(bool writable) { value = (value & ~(1ULL << 1)) | (writable ? (1ULL << 1) : 0); }
    void set_user(bool user) { value = (value & ~(1ULL << 2)) | (user ? (1ULL << 2) : 0); }
    void set_huge(bool huge) { value = (value & ~(1ULL << 7)) | (huge ? (1ULL << 7) : 0); }
} __attribute__((packed));


alignas(4096) PageTableEntry pml4[512];
alignas(4096) PageTableEntry pdpt[512];
alignas(4096) PageTableEntry pd[512];

alignas(16) GDTDescriptor gdt64_descriptor;




static inline bool check_long_mode() {
    uint32_t eax, edx;

    asm volatile (
            "mov $0x80000000, %%eax\n"
            "cpuid\n"
            "mov %%eax, %0\n"
            "mov %%edx, %1\n"
            : "=r"(eax), "=r"(edx)
            :
            : "eax", "ebx", "ecx", "edx"
            );

    if (eax < 0x80000001) return false;

    asm volatile (
            "mov $0x80000001, %%eax\n"
            "cpuid\n"
            "mov %%edx, %0\n"
            : "=r"(edx)
            :
            : "eax", "ebx", "ecx", "edx"
            );

    return (edx & (1 << 29)) != 0;
}


static inline void enable_pae() {
    asm volatile (
            "mov %%cr4, %%eax\n"
            "or $0x20, %%eax\n"
            "mov %%eax, %%cr4\n"
            :
            :
            : "eax"
            );
}


static inline void enable_long_mode() {
    uint32_t eax, edx;


    asm volatile (
            "mov $0xC0000080, %%ecx\n"
            "rdmsr\n"
            "or $0x100, %%eax\n"
            "wrmsr\n"
            : "=a"(eax), "=d"(edx)
            :
            : "ecx"
            );


    asm volatile (
            "mov %%cr0, %%eax\n"
            "or $0x80000000, %%eax\n"
            "mov %%eax, %%cr0\n"
            :
            :
            : "eax"
            );
}


[[noreturn]]
static inline void jump_to_long_mode(uint64_t entry_point) {
    asm volatile (

            "lgdt (%0)\n"


            "push $0x08\n"
            "push %1\n"
            "retf\n"
            :
            : "r"(&gdt64_descriptor), "r"(entry_point)
            : "memory"
            );

    __builtin_unreachable();
}




void setup_paging() {

    memset(pml4, 0, sizeof(pml4));
    memset(pdpt, 0, sizeof(pdpt));
    memset(pd, 0, sizeof(pd));


    pml4[0].set_addr(reinterpret_cast<uint64_t>(pdpt));
    pml4[0].set_present(true);
    pml4[0].set_writable(true);


    pdpt[0].set_addr(reinterpret_cast<uint64_t>(pd));
    pdpt[0].set_present(true);
    pdpt[0].set_writable(true);


    for (int i = 0; i < 512; i++) {
        pd[i].set_addr(i * 0x200000ULL);
        pd[i].set_present(true);
        pd[i].set_writable(true);
        pd[i].set_huge(true);
    }


    asm volatile (
            "mov %0, %%cr3\n"
            :
            : "r"(reinterpret_cast<uint64_t>(pml4))
            : "memory"
            );
}


void init_gdt64() {


    static const uint64_t gdt64[] = {
            0x0000000000000000,
            0x00209A0000000000,
            0x0000920000000000,
    };


    gdt64_descriptor.limit = sizeof(gdt64) - 1;
    gdt64_descriptor.base = reinterpret_cast<uint64_t>(gdt64);
}

extern "C" [[noreturn]] void long_mode_entry();



extern "C" [[noreturn]] void switch_to_64bit() {

    asm volatile ("cli");


    if (!check_long_mode()) {

        asm volatile ("hlt");
        __builtin_unreachable();
    }


    setup_paging();


    enable_pae();


    init_gdt64();


    enable_long_mode();


    jump_to_long_mode(reinterpret_cast<uint64_t>(&long_mode_entry));

    __builtin_unreachable();
}










asm(
        ".section .text\n"
        ".code64\n"
        ".global long_mode_entry\n"
        "long_mode_entry:\n"

        "mov $0x10, %ax\n"
        "mov %ax, %ds\n"
        "mov %ax, %es\n"
        "mov %ax, %fs\n"
        "mov %ax, %gs\n"
        "mov %ax, %ss\n"


        "mov $0x7FFFFFFFFFF, %rsp\n"


        "xor %rax, %rax\n"
        "xor %rbx, %rbx\n"
        "xor %rcx, %rcx\n"
        "xor %rdx, %rdx\n"
        "xor %rsi, %rsi\n"
        "xor %rdi, %rdi\n"
        "xor %rbp, %rbp\n"
        "xor %r8, %r8\n"
        "xor %r9, %r9\n"
        "xor %r10, %r10\n"
        "xor %r11, %r11\n"
        "xor %r12, %r12\n"
        "xor %r13, %r13\n"
        "xor %r14, %r14\n"
        "xor %r15, %r15\n"


        "call kmain64\n"

        ".halt:\n"
        "hlt\n"
        "jmp .halt\n"
        );

#include "../disk/atadriver.cpp"


extern "C" [[noreturn]] void kmain64() {


    uint64_t rax, rbx;

    asm volatile (
            "mov %%cr0, %0\n"
            "mov %%cr4, %1\n"
            : "=r"(rax), "=r"(rbx)
            :
            : "memory"
            );

    ATADriver driver = {};
    driver.read(39, 256, (char*)0x20000);
    INTEL("jmp 0x20000");

    while (true) {
        asm volatile ("hlt");
    }
}


/*
# Компиляция 32-битной части
g++ -m32 -ffreestanding -fno-exceptions -fno-rtti -nostdlib \
    -c boot32.cpp -o boot32.o

# Компиляция 64-битной части
g++ -m64 -ffreestanding -fno-exceptions -fno-rtti -nostdlib \
    -c kernel64.cpp -o kernel64.o

# Линковка с правильными секциями
ld -m elf_i386 -T linker.ld -o kernel.bin boot32.o kernel64.o

# Конвертация в образ
objcopy -O binary kernel.bin kernel.img
*/
#define ydiskans
#define noallocdir

#include "init.cpp"

#ifndef stage2

bool mmstmin() {
    constexpr int count = 32;
    void* p[count]{};
    for (int i = 0; i < count; ++i) {
        p[i] = allocate(128);

    }
    for (int i = count - 1; i >= 0; --i) {
        if (!p[i]) {
            return false;
        }
        free(p[i]);
    }
    return true;
}

extern "C" [[noreturn]] void _start() {
    init();
    //Directory::allocdir();

    KernelOut kout = KernelOut();
    KernelIn kin = KernelIn();
    kout.clear();
    kout << reset;
    kout << WHITE << "XCore";
    kout << reset << " KERNEL v2.0 / Type 'help' for information" << endl;

    //Directory::create("home"_s);
    //Directory::create("home"_s);

    //filesystem::create_dir("home"_s);

    //stop();

    // for (auto i : Directory("home").list()) {
    //     kout << i << endl;
    // }
    // filesystem::create_dir("home/user");
    // filesystem::create_dir("home/user/documents");


    //filesystem::create_file("/home/user/file.txt");


    // auto list = filesystem::list_dir("/home/user");
    // for (auto& item : list) {
    //     kout << item << endl;
    // }


    //filesystem::remove_dir("/home/user/documents");
    //asm volatile("syscall" : : "rax"(0) : "rcx", "r11", "memory");

    // string content = "Zdarova pediki suka naxui";
    // managed<indirect> indirects = {};
    // indirects.push_back({1, 14});
    // indirects.push_back({2, 15});
    // indirects.push_back({4, 16});
    // fragment::write(indirects, 23, content, {});
    // fragment::write(indirects, 32, content, {});
    // fragment::write(indirects, -1, content, {});

    //cmd::execute("disk readsec 14");


    mathtest();
    string com = "";
    while (true) {
        kout << "> ";
        kin.clear();
        com = KernelIn::readLine();
        cmd::execute(com);
    }
}

#endif
#include "init.cpp"

#ifndef stage2

extern "C" [[noreturn]] void _start() {
    init();

    KernelOut kout = KernelOut();
    KernelIn kin = KernelIn();

    kout.clear();
    kout << reset;
    kout << WHITE << "XCore";
    kout << reset << " KERNEL v2.0 / Type 'help' for information" << endl;

    //kout << Math::sin();
    double f = 0;
    for (int i = 0; i < 14; ++i) {
        kout << "UUID4: \"" << uuid4().str() << "\"" << endl;

    }


    filesystem::create_dir("root/home");
    filesystem::create_dir("/home/user");
    filesystem::create_dir("/home/user/documents");


    filesystem::create_file("/home/user/file.txt");


    auto list = filesystem::list_dir("/home/user");
    for (auto& item : list) {
        kout << item << endl;
    }


    //filesystem::remove_dir("/home/user/documents");

    //asm volatile("syscall" : : "rax"(0) : "rcx", "r11", "memory");
    string com = "";
    while (true) {
        kout << "> ";
        kin.clear();
        com = KernelIn::readLine();
        cmd::execute(com);
    }
}

#endif
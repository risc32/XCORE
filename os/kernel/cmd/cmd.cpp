#pragma once

#include "../cpu/cpu.cpp"
#include "../stream/stream.cpp"
#include "../utils/inited.cpp"

typedef const managed<string>& argt;
int checkargs(argt args, int v) {
    KernelOut kout;
    if (args.size() < v) {
        kout << "Incorrect number of arguments";
        return 1;
    }
    return 0;
}

#define CHECK(x) {if (checkargs(args, x)) return 1;}

enum gettype {
    id,
    addr,
    offset
};

void* parseaddr(string s) {
    gettype type;
    memory_block* block = nullptr;
    switch (s[0]) {
        case '@':
            type = gettype::addr;
            block = (memory_block*)to_int(s, 16);
            break;
        case 'x':
            type = gettype::offset;
            block = (memory_block*)(((uint32_t)memory::heap) + (uint32_t)to_int(s, 16));
            break;
        default:
            type = gettype::id;
            block = getbyid(to_int(s));
            break;
    }
    return block;
}

void blockinfo(KernelOut kout, memory_block* block) {
    bool used = block->used;
    kout << setw(10) << dec << block->mbid << hex << used << wstring("@") + to_wstring((uint32_t)block, 16) << wstring("x") + to_wstring((uint32_t)block - (uint32_t)memory::heap, 16) << dec << block->size << endl;
}

void blocktable(KernelOut kout) {
    kout << setw(10) << "dMBID" << "bUSED" << "xADDRESS" << "xOFFSET" << "dSIZE" << endl;

}

void oneblockinfo(memory_block*  block) {
    KernelOut kout;
    blocktable(kout);
    blockinfo(kout, block);
}

#include "basic.cpp"
#include "ram.cpp"
#include "disk.cpp"

struct pcommand {
    string name;
    managed<string> args;
};


struct command {
    string name;
    int (*func)(argt);
};

pcommand parse_cmd(const string& com) {
    pcommand res = {};
    istream s(com);
    KernelOut kout;

    s >> res.name;
    while(s.good()) {
        wstring data = s.getdata();
        res.args.push_back(string(data.data()));
    }

    return res;
    //s.opera;
}



struct cmd {
    static managed<command> commands;

    static void init() {  ///for stacktrace

        commands = managed<command>();
        add_command("reboot", reboot);
        add_command("shutdown", shutdown);
        add_command("help", help);
        add_command("ram", ram);
        add_command("disk", disk);
    }

    static void add_command(const string& name, int (*func)(argt)) {
        commands.push_back(command {
            name, func
        });
    }

    static void execute(string com) {
        KernelOut kout = {};
        //kout << (com.size()) << endl;
        pcommand pcom = parse_cmd(com);




        //kout << string("abcdabcdabcdabcdabcd");

        for (command ccom : commands) {
            if (pcom.name == ccom.name) {

                int f = ccom.func(pcom.args);
                kout << endl << wstring("Command executed with exit code: ") << f << endl;
                return;
            }
        }

        kout << "Command not found: " << pcom.name << endl;
        kout << "Type 'help' for available commands";
    }

private:
    static int help(argt) {
        KernelOut kout;

        int counter = 0;
        for (command com : commands) {
            kout << ++counter << ". " << com.name << endl;
        }

        kout << endl << ("Use 'help <command>' for more information") << endl;
        return 0;
    }


};
managed<command> cmd::commands = managed<command>();

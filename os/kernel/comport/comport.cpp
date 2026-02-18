#include "../utils/inited.cpp"

struct CommunicationPort {
    virtual ~CommunicationPort() = default;

    string name;
    string enter;

    virtual bool commander(string command);

    void start() {
        KernelOut kout;

        kout << WHITE << "XCORE " << reset << name << endl;

        do {
            kout << ' ' << enter << ' ';
        } while (commander(_kcons::console.readLine()));
    }
};


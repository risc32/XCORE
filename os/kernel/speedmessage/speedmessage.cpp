#pragma once

#include "../utils/inited.cpp"
#include "../crypto/crypto.cpp"

typedef uint64_t jpid_t;
typedef uint64_t npid_t;

struct Component;
struct Request;
typedef void(*hook_t)(Request*);

void yield() {
    INTEL ("int 0x20");
}

struct Request {
    bool commit;
    struct {
        Component* receiver;
        jpid_t sender;
    } major;
    struct {
        npid_t receiver = 0;
        npid_t sender = 0;
    } minor;
    hook_t hook;

    void* arguments;
    void* result;

    Request* wait() {
        while (commit) {
                        yield();
        }
        free(arguments);
        return this;
    }
};

struct speedmessage {
    static managed<Component*> components;
    static Component* alloc;
    static Component* cpus;
};

Component* add_component(Component* component) {
    speedmessage::components.push_back(component);
    return component;
}

struct Component {
protected:
    StackPool storage;
    const jpid_t id;
    static jpid_t counter;
public:
    static void init();

    string name{};
    mutex mt;

    virtual ~Component() = default;

    Component(const string &name) : storage{sizeof(Request), 256}, id(counter++), name(name) {
        this->name = name;
    }

    Request* allocreq(Request* req) const {
        auto res = (Request*)req->major.receiver->storage.alloc();
        *res = *req;
        res->major.sender = id;
        res->commit = true;
        return res;
    }

    static void freereq(Request* req) {
        req->major.receiver->storage.free(req);
    }

    Request* send(Request req) const {
        auto r = allocreq(&req);
        yield();
        return r;
    }

    bool sendw(Request req) const {
        send(req)->wait();
        return true;
    }

    bool receive() {
        lock_guard lock{mt};
        for (int i = 0; i < storage.capacity; ++i) {
            if (Request* req = (Request*)storage.memory + i; req->commit) {
                if (req->major.receiver->id == id) {

                    process(req);
                }

                req->commit = false;
                storage.free(req);
            }
        }
        return true;
    }

    virtual void process(Request*) = 0;
};

jpid_t Component::counter = 0;



Component* speedmessage::alloc = nullptr;
Component* speedmessage::cpus = nullptr;
managed<Component*> speedmessage::components{};

#include "alloc.cpp"
#include "cpus.cpp"

void Component::init() {
    speedmessage::components = {};
    speedmessage::alloc = add_component(new Components::Alloc);
    speedmessage::cpus = add_component(new Components::Cpus);
}

[[noreturn]] void update() {
    while (1) {
        for (auto& component : speedmessage::components) {
            component->receive();

        }
    }
}

#include "face.cpp"

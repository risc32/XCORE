#pragma once

#include "speedmessage.cpp"

struct Face {
    Component* c;

    template<typename T> T wait(Request* r) const {
        T res = nullptr;
                return *(T**)r->wait()->result;
    }

    Request* amalloc(const size_t size, void*& res) const {
        return c->send(Request{
            .major = {
                .receiver = speedmessage::alloc
            },
            .minor = {
                .receiver = (int) Components::Alloc::type::malloc,
                .sender = smplow::get_core_id()
            },
            .arguments = new Components::Alloc::arg{.size = size},
            .result = &res,
        });
    }

    Request* afree(void* ptr) const {
        return c->send(Request{
            .major = {
                .receiver = speedmessage::alloc
            },
            .minor = {
                .receiver = (int) Components::Alloc::type::free,
                .sender = smplow::get_core_id()
            },
            .arguments = new Components::Alloc::arg{.ptr = ptr},
                    });
    }

    Request* arealloc(void* ptr, size_t newsize, void*& res) const {
        return c->send(Request{
            .major = {
                .receiver = speedmessage::alloc
            },
            .minor = {
                .receiver = (int) Components::Alloc::type::realloc,
                .sender = smplow::get_core_id()
            },
            .arguments = new Components::Alloc::arg{.size = newsize, .ptr = ptr},
            .result = &res,
        });
    }

    void* malloc(const size_t size) const {
        void* res = nullptr;
        return wait<void*>(amalloc(size, res));
    }

    void free(void* ptr) const {
        afree(ptr)->wait();
    }

    void* realloc(void* ptr, const size_t size) const {
        void* res = nullptr;
        return wait<void*>(arealloc(ptr, size, res));
    }
};

#define CPUFACE Face face{(Components::Cpus*)speedmessage::cpus}

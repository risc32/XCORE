#pragma once

#include "speedmessage.cpp"

namespace Components {
    struct Alloc : Component {
        enum struct type {
            malloc,
            free,
            realloc
        };

        struct arg {
            size_t size;
            void* ptr;
        };

        Alloc() : Component("alloc") {}

        void process(Request* req) override {

            auto t = (type)req->minor.receiver;
            auto a = *(arg*)req->arguments;
            switch (t) {
                case type::malloc:
                    *(void**)req->result = malloc(a.size);
                    break;
                case type::free:
                    free(a.ptr);
                    break;
                case type::realloc:
                    *(void**)req->result = realloc(a.ptr, a.size);
                    break;
            }
        }
    };
}

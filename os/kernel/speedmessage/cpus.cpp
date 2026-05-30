#pragma once

#include "speedmessage.cpp"

namespace Components {
    struct Cpus : Component {
        enum struct type {
            check
        };

        Cpus() : Component("cpus") {}

        void process(Request* req) override {
            auto t = (type)req->minor.receiver;
            switch (t) {
                case type::check:
                    req->result = (void*)"OK";
                    break;
            }
        }


    };
}

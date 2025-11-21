#pragma once

#include "tree.cpp"



struct compability {
    managed<node*> changes;

    void make() {
        for (const auto &item: changes) {
            item;
        }
    }
};
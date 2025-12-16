#pragma once

#include "utils.cpp"
#include "getblocks.cpp"
#include "debugtree.cpp"

managed<string> split(const string& s, char sep) {
    managed<string> res{};
    for (const auto &item: s) {
        if (item != sep) res[res.size()-1] += item;
        else res.push_back("");
    }
    return res;
}
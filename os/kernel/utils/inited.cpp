#pragma once

#include "utils.cpp"
#include "getblocks.cpp"
#include "debugtree.cpp"

managed<string> split(const string& s, char sep) {
    managed<string> res{};
    res.push_back("");
    for (const auto &item: s) {
        if (item != sep) res[res.size()-1] += item;
        else res.push_back("");
    }
    return res;
}

string formatnum(uint64_t num) {
    string result;
    string data = to_string(num);

    int f = 0;
    for (char i : data) {
        if (f == 3) {
            result = string("'") + result;
            f = 0;
        }
        result = string(i) + result;
        f++;
    }

    return result;
}

struct Average {
    uint64_t avg;
    uint64_t count;

    uint64_t max = 1000;

    void add(uint64_t value) {
        avg = (avg * count+1 + value) / ++count;
        count %= max;
    }

    void setmax(uint64_t value) {
        max = value;
    }

    uint64_t get() {
        return avg;
    }
};

template<typename T> managed<char> bytestream(T arg) {
    managed<char> out{};
    uint64_t size = sizeof(arg);
    out.reserve(size);
    for (uint64_t i = 0; i < size; i++) {
        out[i] = ((char*)(&arg))[i];
    }
    return out;
}
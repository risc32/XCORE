#pragma once

#include "stream.cpp"

class Progress {
    Console& console;
    int total;
    int _current;
    int width;
    int scroll;
    uint8_t fg;
    uint8_t bg;

public:
    string label;

    int X;
    int Y;

    Progress(int total, Console &console, int width = 20, const string& lbl = "")
            : width(width), _current(0), total(total), console(console), label(lbl) {







        place();
    }

    Progress(int total, int width = 20, const string& lbl = "")
            : width(width), _current(0), total(total), console(_kcons::console), label(lbl) {






        place();
    }

    [[nodiscard]] int get_filled_count() const {
        if (total == 0) return 0;
        return (float)_current / total * width;
    }

    [[nodiscard]] int get_empty_count() const {
        return width - get_filled_count();
    }

    void place() const {

        wstring bar = wstring(get_filled_count(), L'=') + wstring(get_empty_count(), L' ');


        if (_current < total && get_filled_count() < width) {
            if (bar.size() > get_filled_count()) {
                bar[get_filled_count()] = L'>';
            }
        }


        wstring full_string;




        full_string += wstring(L"[") + bar + wstring(L"] ");


        int percent = total > 0 ? (float)_current / total * 100 : 0;
        full_string += to_wstring(percent) + wstring(L"%");


        if (width >= 15) {
            full_string += wstring(L" (") + to_wstring(_current) + wstring(L"/") + to_wstring(total) + wstring(L")");
        }
        if (!label.empty()) {
            full_string += wstring(L" ") + wstring(label.data());
        }








    }

    void set(int current) {
        _current = current;
        if (_current > total) _current = total;
        if (_current < 0) _current = 0;
        place();
    }

    void add(int a) {
        set(_current + a);
    }

    void increment() {
        add(1);
    }

    void finish() {
        set(total);
    }

    void reset() {
        set(0);
    }

    [[nodiscard]] int current() const {
        return _current;
    }

    [[nodiscard]] bool is_complete() const {
        return _current >= total;
    }

    [[nodiscard]] float progress() const {
        if (total == 0) return 0.0f;
        return (float)_current / total;
    }
};


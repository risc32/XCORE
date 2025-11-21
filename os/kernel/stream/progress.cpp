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
        scroll = console.scrolled;
        X = console.cursor_x;
        Y = console.cursor_y;
        fg = console.current_fg_color;
        bg = console.current_bg_color;

        console.cursor_x += width + 12 + lbl.size() + count_digits(total)*2; // Место для прогресс-бара и процентов
        place();
    }

    Progress(int total, int width = 20, const string& lbl = "")
            : width(width), _current(0), total(total), console(_kcons::console), label(lbl) {
        scroll = console.scrolled;
        X = console.cursor_x;
        Y = console.cursor_y;
        fg = console.current_fg_color;
        bg = console.current_bg_color;
        console.cursor_x += width + 12 + lbl.size() + count_digits(total)*2;
        place();
    }

    [[nodiscard]] int get_filled_count() const {
        if (total == 0) return 0;
        return (float)_current / total * width; // NOLINT(*-narrowing-conversions)
    }

    [[nodiscard]] int get_empty_count() const {
        return width - get_filled_count();
    }

    void place() const {
        // Строим прогресс-бар
        wstring bar = wstring(get_filled_count(), L'=') + wstring(get_empty_count(), L' ');

        // Добавляем стрелку если не полный
        if (_current < total && get_filled_count() < width) {
            if (bar.size() > get_filled_count()) {
                bar[get_filled_count()] = L'>';
            }
        }

        // Формируем полную строку
        wstring full_string;

        // Добавляем метку если есть


        full_string += wstring(L"[") + bar + wstring(L"] ");

        // Добавляем проценты
        int percent = total > 0 ? (float)_current / total * 100 : 0; // NOLINT(*-narrowing-conversions)
        full_string += to_wstring(percent) + wstring(L"%");

        // Добавляем числа если есть место
        if (width >= 15) {
            full_string += wstring(L" (") + to_wstring(_current) + wstring(L"/") + to_wstring(total) + wstring(L")");
        }
        if (!label.empty()) {
            full_string += wstring(L" ") + wstring(label.data());
        }

        uint8_t bfg = console.current_fg_color;
        uint8_t bbg = console.current_bg_color;

        console.set_color((VGAColor)fg, (VGAColor)bg);
        // Отображаем
        console.place(string(full_string.data()).data(), X, Y-(console.scrolled - scroll));
        console.set_color((VGAColor)bfg, (VGAColor)bbg);
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
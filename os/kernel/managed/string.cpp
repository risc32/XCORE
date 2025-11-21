#pragma once

#include "managed.cpp"

struct wstring;

struct string : public managed<char> {
    string() : managed<char>() {
        nullend = true;
        // Гарантируем нуль-терминатор
        _data[0] = '\0';
    }

    string(const char *s) : managed<char>() {
        nullend = true;
        if (s) {
            while (*s != '\0') {
                push_back(*(s++));
            }
        }
        // Гарантируем нуль-терминатор
        if (_size < _capacity) {
            _data[_size] = '\0';
        }
    }

    string(const wchar_t *s) : managed<char>() {
        nullend = true;
        if (s) {
            while (*s != 0) {
                // Конвертируем wchar_t в char (простое приведение)
                push_back(static_cast<char>(*(s++)));
            }
        }
        // Гарантируем нуль-терминатор
        if (_size < _capacity) {
            _data[_size] = '\0';
        }
    }

    // Конструктор копирования
    string(const string& other) : managed<char>(other) {
        nullend = true;
    }

    string& operator=(const string& other) {
        managed<char>::operator=(other);
        nullend = true;
        return *this;
    }

    using managed<char>::operator==;

    bool operator==(const char* other) const {
        if (!other) return _size == 0;

        const char* a = _data;
        const char* b = other;
        while (*a && *b && *a == *b) {
            a++;
            b++;
        }
        return (*a == '\0' && *b == '\0');
    }

    bool operator!=(const char* other) const {
        return !(*this == other);
    }

    string(int count, char elem) : managed<char>() {
        nullend = true;
        for (int i = 0; i < count; ++i) {
            push_back(elem);
        }
        // Гарантируем нуль-терминатор
        if (_size < _capacity) {
            _data[_size] = '\0';
        }
    }

    template<typename T>
    string operator+(T other) const {
        string result(*this);
        result += other;
        return result;
    }

    string(char c) : managed<char>() {
        nullend = true;
        push_back(c);
        // Гарантируем нуль-терминатор
        if (_size < _capacity) {
            _data[_size] = '\0';
        }
    }

    template<size_t N>
    string(const char (&array)[N]) : managed<char>() {
        nullend = true;
        for (size_t i = 0; i < N && array[i] != '\0'; i++) {
            push_back(array[i]);
        }
        // Гарантируем нуль-терминатор
        if (_size < _capacity) {
            _data[_size] = '\0';
        }
    }

    // Метод для получения C-style строки
    const char* c_str() const {
        return _data;
    }
};

struct wstring : public managed<wchar_t> {
    wstring() : managed<wchar_t>() {
        nullend = true;
        // Гарантируем нуль-терминатор
        if (_capacity > 0) {
            _data[0] = L'\0';
        }
    }

    wstring(const wchar_t *s) : managed<wchar_t>() {
        nullend = true;
        if (s) {
            while (*s != L'\0') {
                push_back(*(s++));
            }
        }
        // Гарантируем нуль-терминатор
        if (_size < _capacity) {
            _data[_size] = L'\0';
        }
    }

    wstring(const char *s) : managed<wchar_t>() {
        nullend = true;
        if (s) {
            while (*s != '\0') {
                // Конвертируем char в wchar_t
                push_back(static_cast<wchar_t>(*(s++)));
            }
        }
        // Гарантируем нуль-терминатор
        if (_size < _capacity) {
            _data[_size] = L'\0';
        }
    }

    // Конструктор копирования
    wstring(const wstring& other) : managed<wchar_t>(other) {
        nullend = true;
    }

    wstring& operator=(const wstring& other) {
        managed<wchar_t>::operator=(other);
        nullend = true;
        return *this;
    }

    using managed<wchar_t>::operator==;
    using managed<wchar_t>::operator+;

    bool operator==(const char* other) const {
        return *this == wstring(other);
    }

    bool operator==(const wchar_t* other) const {
        return *this == wstring(other);
    }

    template<typename T>
    wstring operator+(T other) {
        wstring result(*this);
        result += other;
        return result;
    }

    wstring(const string &s) : managed<wchar_t>() {
        nullend = true;
        for (size_t i = 0; i < s.size(); i++) {
            push_back(static_cast<wchar_t>(s[i]));
        }
        // Гарантируем нуль-терминатор
        if (_size < _capacity) {
            _data[_size] = L'\0';
        }
    }

    wstring(int count, wchar_t elem) : managed<wchar_t>() {
        nullend = true;
        for (int i = 0; i < count; ++i) {
            push_back(elem);
        }
        // Гарантируем нуль-терминатор
        if (_size < _capacity) {
            _data[_size] = L'\0';
        }
    }

    wstring(wchar_t c) : managed<wchar_t>() {
        nullend = true;
        push_back(c);
        // Гарантируем нуль-терминатор
        if (_size < _capacity) {
            _data[_size] = L'\0';
        }
    }

    template<size_t N>
    wstring(const wchar_t (&array)[N]) : managed<wchar_t>() {
        nullend = true;
        for (size_t i = 0; i < N && array[i] != L'\0'; i++) {
            push_back(array[i]);
        }
        // Гарантируем нуль-терминатор
        if (_size < _capacity) {
            _data[_size] = L'\0';
        }
    }

    // Метод для получения C-style широкой строки
    const wchar_t* c_str() const {
        return _data;
    }
};

string operator "" _s(const char *s) {
    return string(s);
}

string to_string(int val, int base = 10) {
    char buf[32]; // Увеличим буфер для безопасности
    // Предполагается, что itoa существует в вашей системе
    return string(itoa(val, buf, base));
}

wstring to_wstring(int val, int base = 10) {
    char buf[32];
    return wstring(itoa(val, buf, base));
}

int to_int(const string &s, int base = 10) {
    return atoi(s.c_str(), base);
}

int to_int(const wstring &s, int base = 10) {
    // Конвертируем wstring в string и затем в int
    string temp;
    for (size_t i = 0; i < s.size(); i++) {
        temp.push_back(static_cast<char>(s[i]));
    }
    return atoi(temp.c_str(), base);
}
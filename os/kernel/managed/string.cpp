#pragma once

#include "managed.cpp"

struct wstring;

struct string : public managed<char> {
    string() : managed<char>() {
        nullend = true;

        _data[0] = '\0';
    }

    string(const char *s) : managed<char>() {
        nullend = true;
        if (s) {
            while (*s != '\0') {
                push_back(*(s++));
            }
        }

        if (_size < _capacity) {
            _data[_size] = '\0';
        }
    }

    string(const wchar_t *s) : managed<char>() {
        nullend = true;
        if (s) {
            while (*s != 0) {

                push_back(static_cast<char>(*(s++)));
            }
        }

        if (_size < _capacity) {
            _data[_size] = '\0';
        }
    }


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

        if (_size < _capacity) {
            _data[_size] = '\0';
        }
    }


    const char* c_str() const {
        return _data;
    }
};

struct wstring : public managed<wchar_t> {
    wstring() : managed<wchar_t>() {
        nullend = true;

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

        if (_size < _capacity) {
            _data[_size] = L'\0';
        }
    }

    wstring(const char *s) : managed<wchar_t>() {
        nullend = true;
        if (s) {
            while (*s != '\0') {

                push_back(static_cast<wchar_t>(*(s++)));
            }
        }

        if (_size < _capacity) {
            _data[_size] = L'\0';
        }
    }


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

        if (_size < _capacity) {
            _data[_size] = L'\0';
        }
    }

    wstring(int count, wchar_t elem) : managed<wchar_t>() {
        nullend = true;
        for (int i = 0; i < count; ++i) {
            push_back(elem);
        }

        if (_size < _capacity) {
            _data[_size] = L'\0';
        }
    }

    wstring(wchar_t c) : managed<wchar_t>() {
        nullend = true;
        push_back(c);

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

        if (_size < _capacity) {
            _data[_size] = L'\0';
        }
    }


    const wchar_t* c_str() const {
        return _data;
    }
};

string operator "" _s(const char *s) {
    return string(s);
}

string to_string(int val, int base = 10) {
    char buf[32];

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

    string temp;
    for (size_t i = 0; i < s.size(); i++) {
        temp.push_back(static_cast<char>(s[i]));
    }
    return atoi(temp.c_str(), base);
}
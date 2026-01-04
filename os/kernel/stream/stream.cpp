#pragma once

#include "../types/types.cpp"
#include "../managed/managed.cpp"

struct _workersetw {
    int width;
};

void debug(const string&, bool ac = false);

wstring swidth(wstring s, int w) {
    return s + wstring(max(w - s.size(), 0), ' ');
}

class stream_base {
protected:
    wstring buffer;
    virtual void _flush() = 0;
public:
    int width = 0;
    int base = 10;
    bool boolalpha = true;

    stream_base() : buffer(L"") {}
    explicit stream_base(const wchar_t* buffer) : buffer(buffer) {}
    explicit stream_base(const wstring& ws) : buffer(ws) {}

    bool eof() {  ///for stacktrace

        return buffer.empty();
    }
    bool good() {  ///for stacktrace

        return !eof();
    }

    virtual void clear() {  ///for stacktrace

        buffer.clear();
    }

    void putchar(wchar_t c) {
        this->buffer += swidth(static_cast<wstring>(c), width);
    }
    void putchar(char c) {
        this->buffer += swidth(static_cast<wstring>(c), width);
    }

    void putstr(const wchar_t* s) {
        this->buffer += swidth(wstring(s), width);
    }
    void putstr(const wstring& s) {
        this->buffer += swidth(wstring(s), width);
    }
    void putstr(const char* s) {
        this->buffer += swidth(wstring(s), width);
    }
    void putstr(const string& s) {
        this->buffer += swidth(wstring(s), width);
    }

    void remchar() {  ///for stacktrace

        this->buffer.pop_back();
    }

    void flush() {  ///for stacktrace

        _flush();
        buffer.clear();
    };

    wstring& getbuffer() {  ///for stacktrace

        return this->buffer;
    }
};

class ostream : public stream_base {
    void _flush() override {
    }
public:
    ostream() : stream_base() {  ///for stacktrace
}
    explicit ostream(const wchar_t* buffer) : stream_base(buffer) {}
    explicit ostream(const wstring& ws) : stream_base(ws) {}

    ostream& operator<<(const wchar_t* b) {
        debug(__PRETTY_FUNCTION__ );

        putstr(wstring(b));
        flush();
        return *this;
    }

    ostream& operator<<(const wchar_t b) {
        debug(__PRETTY_FUNCTION__ );

        putstr(wstring(b));
        flush();
        return *this;
    }

    ostream& operator<<(const char* b) {
        debug(__PRETTY_FUNCTION__ );

        putstr(wstring(b));
        flush();
        return *this;
    }

    ostream& operator<<(const char b) {
        debug(__PRETTY_FUNCTION__ );

        putstr(wstring(b));
        flush();
        return *this;
    }

    ostream& operator<<(const int b) {
        debug(__PRETTY_FUNCTION__ );

        putstr(to_wstring(b, base));
        flush();
        return *this;
    }

    template<typename T> ostream& operator<<(T* b) {
        debug(__PRETTY_FUNCTION__ );

        putstr(to_wstring((uint64_t )b, 16));
        flush();
        return *this;
    }

    ostream& operator<<(const unsigned int b) {
        debug(__PRETTY_FUNCTION__ );

        putstr(to_wstring(b, base));
        flush();
        return *this;
    }

    ostream& operator<<(const long long b) {
        debug(__PRETTY_FUNCTION__ );

        putstr(to_wstring(b, base));
        flush();
        return *this;
    }

    ostream& operator<<(const unsigned long long b) {
        debug(__PRETTY_FUNCTION__ );

        putstr(to_wstring(b, base));
        flush();
        return *this;
    }

    ostream& operator<<(const wstring& b) {
        debug(__PRETTY_FUNCTION__ );

        putstr(wstring(b));
        flush();
        return *this;
    }

    ostream& operator<<(const string& b) {
        debug(__PRETTY_FUNCTION__ );

        putstr(wstring(b));
        flush();
        return *this;
    }

    ostream& operator<<(const bool b) {
        debug(__PRETTY_FUNCTION__ );

        if (boolalpha) {
            putstr(wstring(b ? L"true" : L"false"));
        } else {
            putstr(b ? L"1" : L"0");
        }
        flush();
        return *this;
    }

    ostream& operator<<(void(*b)(ostream&)) {
        debug(__PRETTY_FUNCTION__ );

        b(*this);
        return *this;
    }

    ostream& operator<<(_workersetw set) {
        width = set.width;
        return *this;
    }




//



};

class istream : public stream_base {
    void _flush() override {
    }
public:
    istream() : stream_base() {  ///for stacktrace
}
    explicit istream(const wchar_t* buffer) : stream_base(buffer) {}
    explicit istream(const wstring& ws) : stream_base(ws) {}

    wstring getdata() {  ///for stacktrace

        wstring data = "";
        bool end = false;
        auto* s = this->buffer.begin();
        while(*s != L' ' && *s != L'\n' && s != this->buffer.end()) {
            data += *s;
            s++;
        }
        end = s == this->buffer.end();

        for (int i = 0; i < data.size(); ++i) {
            buffer.remove(0);
        }
        if (!end) {
            while(!eof() && *buffer.begin() == ' ') buffer.remove(0);
        }

        return data;
    }

    istream& operator>>(wchar_t& b) {
        b = this->buffer[0];
        this->buffer.remove(0);
        return *this;
    }

    istream& operator>>(char& b) {
        b = this->buffer[0];
        this->buffer.remove(0);
        return *this;
    }

    istream& operator>>(int& b) {
        b = to_int(getdata(), base);
        return *this;
    }

    istream& operator>>(wstring& b) {
        b = getdata();
        return *this;
    }

    istream& operator>>(string& b) {
        b = string(getdata().data());
        return *this;
    }

    istream& operator>>(void(*b)(istream&)) {
        b(*this);
        return *this;
    }

    template<typename T>
    istream& operator>>(T& value) {
        //if(istream::eof()) istream::buffer += wstring(_kcons::console.readLine());
        operator>>(value);
        return *this;
    }
};

struct wsstream : public ostream {
    wstring str;
protected:
    void _flush() override {
        str += buffer;
        buffer.clear();
    }
};

class iostream : public istream, public ostream {
public:
    iostream() : istream(), ostream() {  ///for stacktrace
}
};

void endl(ostream& os) {
    os.getbuffer()+=L"\n";
    os.flush();
}
void flush(ostream& os) {
    os.flush();
}
void flush(istream& os) {
    os.flush();
}
void ends(ostream& os) {
    os << 0;
}

void hex(ostream& os) {
    os.base = 16;
}
void hex(istream& os) {
    os.base = 16;
}
void dec(ostream& os) {
    os.base = 10;
}
void dec(istream& os) {
    os.base = 10;
}
void oct(ostream& os) {
    os.base = 8;
}
void oct(istream& os) {
    os.base = 8;
}
void bin(ostream& os) {
    os.base = 2;
}
void bin(istream& os) {
    os.base = 2;
}

void boolalpha(ostream& os) {
    os.boolalpha = true;
}
void noboolalpha(ostream& os) {
    os.boolalpha = false;
}

string space(int count) {
    return {count, ' '};
}

wstring wspace(int count) {
    return {count, ' '};
}

auto setw(int width) {
    return _workersetw{width};
}

#include "kcons.cpp"
#include "progress.cpp"
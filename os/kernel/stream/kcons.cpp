#pragma once

#include "stream.cpp"
#include "../graphics/text/console.hpp"
#include "cstatic.cpp"
#include "../managed/managed.cpp"
#include "../graphics/graphics.cpp"

void debug(const string& s, bool ac) {
#ifdef DEBUG
    VGAColor c = (VGAColor)_kcons::console.current_fg_color;
    _kcons::console.set_foreground_color(DARK_GRAY);
    _kcons::console.writeLine("", true);
    _kcons::console.writeLine(s.data(), true);
    if (ac) _kcons::console.readChar();
    _kcons::console.set_foreground_color(c);
#endif
}

class KernelIn : public istream {
protected:
    void _flush() override {}
public:
    KernelIn () : istream() {  ///for stacktrace
}

    template<typename T>
    KernelIn& operator>>(T& value) {
        if(eof()) buffer += wstring(readLine());
        istream::operator>>(value);
        return *this;
    }
    KernelIn& operator>>(void(*b)(istream&)) {
        b(*this);
        return *this;
    }

    static string readLine() {  ///for stacktrace

        return _kcons::console.readLine();
    }

    wstring* ibuf() {  ///for stacktrace

        return &(buffer);
    }
};

struct _workersetc {
    bool bg;
    Color color;
};

class KernelOut : public ostream {
protected:
    //Console console = Console();

    void _flush() override {
        _kcons::console.write(buffer.data(), true);
        //Screen::out(buffer.data());
        //Screen::frame();
    }
public:
    KernelOut () : ostream() {  ///for stacktrace
}

    template<typename T>
    KernelOut& operator<<(T value) {
        debug(__PRETTY_FUNCTION__ );
        ostream::operator<<(value);
        return *this;
    }
    KernelOut& operator<<(void(*b)(ostream&)) {
        b(*this);
        return *this;
    }
    KernelOut& operator<<(_workersetc col) {
        if (col.bg) _kcons::console.set_background_color(col.color);
        else _kcons::console.set_foreground_color(col.color);
        return *this;
    }
    KernelOut& operator<<(Color col) {
        _kcons::console.set_foreground_color(col);
        return *this;
    }

    wstring* obuf() {  ///for stacktrace

        return &(buffer);
    }

    void clear() override {  ///for stacktrace
        //Screen::clear();
        //Screen::clear(0, Screen::info);
        _kcons::console.clear();
        buffer.clear();
    }
};

_workersetc fg(Color color) {
    return {false, color};
}

_workersetc bg(Color color) {
    return {true, color};
}

void reset(ostream& kernelOut) {
    _kcons::console.reset_color();
}

//KernelOut kout;
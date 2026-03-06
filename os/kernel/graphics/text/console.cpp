#pragma once

#include "../graphics.cpp"

Console::Console() : shift_pressed(false), ctrl_pressed(false), alt_pressed(false), caps_lock(false) {
}

void Console::init() {

}

void Console::write(char c, bool both) {
    Screen::out(c, both);
}

void Console::write(const char *s, bool both) {
    Screen::out(s, both);
}

void Console::write(const wchar_t *s, bool both) {
    Screen::out(s, both);
}

void Console::writeLine(const char *s, bool both) {
    Screen::out(s, true);
    Screen::out("\n", both);
}

void Console::write(int i) {
    char buffer[10];
    Screen::out(itoa(i, buffer, 10), true);
}

void Console::writeHex(int i) {
    char buffer[10];
    Screen::out(itoa(i, buffer, 16), true);
}

void Console::write(int i, int base) {
    char buffer[10];
    Screen::out(itoa(i, buffer, base), true);
}

void Console::place(char c) {
    Screen::out(c, true);
}

void Console::clear() {
    Screen::clear(Screen::bg, Screen::buffer);
    Screen::clear(Screen::bg, Screen::info);
}

void Console::set_color(Color fg, Color bg) {
    Screen::fg = fg;
    Screen::bg = bg;
}

void Console::set_foreground_color(Color color) {
    Screen::fg = color;
}

void Console::set_background_color(Color color) {
    Screen::bg = color;
}

void Console::set_font(string name) {
    Screen::set_font(name);
}

void Console::reset_color() {
    Screen::fg = LIGHT_GRAY;
    Screen::bg = BLACK;
}

bool Console::hasInput() {
    unsigned char status = inb(0x64);
    return (status & 0x01) != 0;
}

void Console::handle_scancode(unsigned char scancode) {
    bool key_released = (scancode & 0x80) != 0;
    unsigned char key_code = scancode & 0x7F;

    if (key_released) {
        switch (key_code) {
            case 0x2A:
            case 0x36:
                shift_pressed = false;
                break;
            case 0x1D:
                ctrl_pressed = false;
                break;
            case 0x38:
                alt_pressed = false;
                break;
            default: ;
        }
    } else {
        switch (key_code) {
            case 0x2A:
            case 0x36:
                shift_pressed = true;
                break;
            case 0x1D:
                ctrl_pressed = true;
                break;
            case 0x38:
                alt_pressed = true;
                break;
            case 0x3A:
                caps_lock = !caps_lock;
                break;
            default: ;
        }
    }
}

char Console::get_char(unsigned char scancode) {
    if (scancode >= 128) return 0;

    char ch = shift_pressed ? scancode_shift[scancode] : scancode_normal[scancode];


    if (caps_lock) {
        if (ch >= 'a' && ch <= 'z') {
            ch = shift_pressed ? ch : ch - 32;
        } else if (ch >= 'A' && ch <= 'Z') {
            ch = shift_pressed ? ch + 32 : ch;
        }
    }

    return ch;
}


bool Console::is_arrow_key(unsigned char scancode, int &arrow_type) {
    if (scancode == 0xE0) {
        unsigned char next_scancode = inb(0x60);
        switch (next_scancode & 0x7F) {
            case 0x48: arrow_type = 1;
                return true;
            case 0x50: arrow_type = 2;
                return true;
            case 0x4B: arrow_type = 3;
                return true;
            case 0x4D: arrow_type = 4;
                return true;
            default: return false;
        }
    }
    return false;
}

char Console::readChar() {
    while (true) {
        while (!hasInput()) {
            asm volatile ("pause");
        }

        unsigned char scancode = inb(0x60);


        handle_scancode(scancode);


        if (scancode & 0x80) {
            key_processed = true;
            continue;
        }


        int arrow_type = 0;
        if (scancode == 0xE0) {
            unsigned char next_scancode = inb(0x60);
            if (is_arrow_key(0xE0, arrow_type)) {
                switch (arrow_type) {
                    case 1: return 0x11;
                    case 2: return 0x12;
                    case 3: return 0x13;
                    case 4: return 0x14;
                }
            }
            continue;
        }


        if (scancode == last_scancode && !key_processed) {
            continue;
        }


        char c = get_char(scancode);
        if (c != 0) {
            last_scancode = scancode;
            key_processed = false;
            return c;
        }

        last_scancode = 0;
        key_processed = true;
    }
}

string Console::readLine() {
    string line;
    while (true) {
        Screen::outfixed('_', true);

        char c = readChar();
        if (c == '\n') {
            break;
        } else if (c == '\b') {
            if (!line.empty()) {
                line.pop_back();
                Screen::outfixed('\0', true);

                write("\b \b", true);
            }
        } else {
            line += c;
            write(c);
        }
    }
    Screen::outfixed('\0', true);
    write('\n');

    return line;
}

const char Console::scancode_normal[128] = {
    0, 27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
    '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
    0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',
    0, '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0,
    '*', 0, ' ', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, '-', 0, 0, 0, '+', 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

const char Console::scancode_shift[128] = {
    0, 27, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', '\b',
    '\t', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n',
    0, 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '"', '~',
    0, '|', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?', 0,
    '*', 0, ' ', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, '-', 0, 0, 0, '+', 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

unsigned char Console::last_scancode = 0;
bool Console::key_processed = true;
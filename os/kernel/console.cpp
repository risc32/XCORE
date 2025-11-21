#pragma once

#include "utils/utils.cpp"

class string;


enum VGAColor {
    BLACK = 0,
    BLUE = 1,
    GREEN = 2,
    CYAN = 3,
    RED = 4,
    MAGENTA = 5,
    BROWN = 6,
    LIGHT_GRAY = 7,
    DARK_GRAY = 8,
    LIGHT_BLUE = 9,
    LIGHT_GREEN = 10,
    LIGHT_CYAN = 11,
    LIGHT_RED = 12,
    LIGHT_MAGENTA = 13,
    YELLOW = 14,
    WHITE = 15
};


char wchar_to_cp866(wchar_t wc) {

    if (wc < 0x80) {
        return static_cast<char>(wc);
    }



    switch (wc) {
        case L'А': return 0x80; case L'а': return 0xA0;
        case L'Б': return 0x81; case L'б': return 0xA1;
        case L'В': return 0x82; case L'в': return 0xA2;
        case L'Г': return 0x83; case L'г': return 0xA3;
        case L'Д': return 0x84; case L'д': return 0xA4;
        case L'Е': return 0x85; case L'е': return 0xA5;
        case L'Ж': return 0x86; case L'ж': return 0xA6;
        case L'З': return 0x87; case L'з': return 0xA7;
        case L'И': return 0x88; case L'и': return 0xA8;
        case L'Й': return 0x89; case L'й': return 0xA9;
        case L'К': return 0x8A; case L'к': return 0xAA;
        case L'Л': return 0x8B; case L'л': return 0xAB;
        case L'М': return 0x8C; case L'м': return 0xAC;
        case L'Н': return 0x8D; case L'н': return 0xAD;
        case L'О': return 0x8E; case L'о': return 0xAE;
        case L'П': return 0x8F; case L'п': return 0xAF;

        case L'Р': return 0x90; case L'р': return 0xE0;
        case L'С': return 0x91; case L'с': return 0xE1;
        case L'Т': return 0x92; case L'т': return 0xE2;
        case L'У': return 0x93; case L'у': return 0xE3;
        case L'Ф': return 0x94; case L'ф': return 0xE4;
        case L'Х': return 0x95; case L'х': return 0xE5;
        case L'Ц': return 0x96; case L'ц': return 0xE6;
        case L'Ч': return 0x97; case L'ч': return 0xE7;
        case L'Ш': return 0x98; case L'ш': return 0xE8;
        case L'Щ': return 0x99; case L'щ': return 0xE9;
        case L'Ъ': return 0x9A; case L'ъ': return 0xEA;
        case L'Ы': return 0x9B; case L'ы': return 0xEB;
        case L'Ь': return 0x9C; case L'ь': return 0xEC;
        case L'Э': return 0x9D; case L'э': return 0xED;
        case L'Ю': return 0x9E; case L'ю': return 0xEE;
        case L'Я': return 0x9F; case L'я': return 0xEF;

        case L'Ё': return 0xF0; case L'ё': return 0xF1;

        default: return '?';
    }
}

inline void int_to_hex(unsigned int number, char *buffer) {
    const char hex_chars[] = "0123456789ABCDEF";
    int i = 0;
    int shift = 28;

    while (shift > 0) {
        int digit = (number >> shift) & 0xF;
        if (digit != 0) break;
        shift -= 4;
    }

    while (shift >= 0) {
        int digit = (number >> shift) & 0xF;
        buffer[i++] = hex_chars[digit];
        shift -= 4;
    }

    if (i == 0) buffer[i++] = '0';
    buffer[i] = '\0';
}

struct Console {
    volatile unsigned short *buffer;
    int cursor_x, cursor_y;

    uint8_t current_fg_color;
    uint8_t current_bg_color;

    static char scancode_normal[128];
    static char scancode_shift[128];
    static bool shift_pressed;
    static bool caps_lock;
    static bool ctrl_pressed;
    static bool alt_pressed;
    static unsigned char last_scancode;
    static bool key_processed;
    int scrolled = 0;

    explicit Console(unsigned short *buffer) {
        this->buffer = buffer;
        cursor_x = 0;
        cursor_y = 0;
        shift_pressed = false;
        caps_lock = false;
        ctrl_pressed = false;
        alt_pressed = false;
        last_scancode = 0;
        key_processed = true;

        current_fg_color = LIGHT_GRAY;
        current_bg_color = BLACK;
        init_scancode_tables();
    }

    explicit Console(int bufaddr = 0xB8000) {
        buffer = (volatile unsigned short *) bufaddr;
        cursor_x = 0;
        cursor_y = 0;


        current_fg_color = LIGHT_GRAY;
        current_bg_color = BLACK;

        shift_pressed = false;
        caps_lock = false;
        ctrl_pressed = false;
        alt_pressed = false;
        last_scancode = 0;
        key_processed = true;
        init_scancode_tables();
    }


    void set_foreground_color(VGAColor color) {
        current_fg_color = color;
    }

    void set_background_color(VGAColor color) {
        current_bg_color = color;
    }

    void set_color(VGAColor fg, VGAColor bg) {
        current_fg_color = fg;
        current_bg_color = bg;
    }

    void reset_color() {  ///for stacktrace

        current_fg_color = LIGHT_GRAY;
        current_bg_color = BLACK;
    }


    uint8_t get_attribute() const {
        return (current_bg_color << 4) | (current_fg_color & 0x0F);
    }

private:
    void init_scancode_tables() {  ///for stacktrace

        const char normal_init[128] = {
                0, 27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
                '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
                0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',
                0, '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0,
                '*', 0, ' ', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                0, 0, 0, 0, 0, 0, '-', 0, 0, 0, '+', 0, 0, 0, 0, 0,
                0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
        };

        const char shift_init[128] = {
                0, 27, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', '\b',
                '\t', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n',
                0, 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '"', '~',
                0, '|', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?', 0,
                '*', 0, ' ', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                0, 0, 0, 0, 0, 0, '-', 0, 0, 0, '+', 0, 0, 0, 0, 0,
                0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
        };

        for (int i = 0; i < 128; i++) {
            scancode_normal[i] = normal_init[i];
            scancode_shift[i] = shift_init[i];
        }
    }

    void show_cursor() {  ///for stacktrace

        if (cursor_x < 80 && cursor_y < 25) {
            int offset = cursor_y * 80 + cursor_x;
            buffer[offset] = (0x70 << 8) | ' ';
        }
    }

    void hide_cursor() {  ///for stacktrace

        if (cursor_x < 80 && cursor_y < 25) {
            int offset = cursor_y * 80 + cursor_x;
            buffer[offset] = (0x07 << 8) | ' ';
        }
    }

public:
    static unsigned char inb(unsigned short port) {
        unsigned char result;

        asm volatile (
                "movw %1, %%dx\n\t"
                "inb %%dx, %%al"
                : "=a" (result)
                : "r" (port)
                : "dx"
                );
        return result;
    }



    void handle_scancode(unsigned char scancode) {
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
            }
        }
    }

    char get_char(unsigned char scancode) {
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


    bool is_arrow_key(unsigned char scancode, int &arrow_type) {

        if (scancode == 0xE0) {

            unsigned char next_scancode = inb(0x60);
            switch (next_scancode & 0x7F) {
                case 0x48: arrow_type = 1; return true;
                case 0x50: arrow_type = 2; return true;
                case 0x4B: arrow_type = 3; return true;
                case 0x4D: arrow_type = 4; return true;
                default: return false;
            }
        }
        return false;
    }

    string readLine();

    void scroll() {  ///for stacktrace

        uint8_t attribute = get_attribute();
        for (int i = 0; i < 80 * 24; i++) {
            buffer[i] = buffer[i + 80];
        }
        for (int i = 80 * 24; i < 80 * 25; i++) {
            buffer[i] = (attribute << 8) | ' ';
        }
        scrolled++;
    }

    void clear() {  ///for stacktrace

        uint8_t attribute = get_attribute();

        for (int i = 0; i < 80 * 25; i++) {
            buffer[i] = (attribute << 8) | ' ';
        }
        cursor_x = 0;
        cursor_y = 0;
    }

    void place(const char *str, int x, int y) {
        uint8_t attribute = get_attribute();

        int offset = y * 80 + x;
        for (int i = 0; str[i] != '\0'; i++) {
            buffer[offset] = (attribute << 8) | str[i];
            offset++; // Добавляем эту строку!
        }
    }
    void write(const char *str) {
        uint8_t attribute = get_attribute();
        for (int i = 0; str[i] != '\0'; i++) {
            if (str[i] == '\n') {
                cursor_x = 0;
                cursor_y++;
                if (cursor_y >= 25) {
                    scroll();
                    cursor_y = 24;
                }
            } else {
                int offset = cursor_y * 80 + cursor_x;
                buffer[offset] = (attribute << 8) | str[i];

                cursor_x++;
                if (cursor_x >= 80) {
                    cursor_x = 0;
                    cursor_y++;
                    if (cursor_y >= 25) {
                        scroll();
                        cursor_y = 24;
                    }
                }
            }
        }
    }
    void write(const wchar_t *str) {
        uint8_t attribute = get_attribute();

        for (int i = 0; str[i] != L'\0'; i++) {
            if (str[i] == L'\n') {
                cursor_x = 0;
                cursor_y++;
                if (cursor_y >= 25) {
                    scroll();
                    cursor_y = 24;
                }
            } else {
                int offset = cursor_y * 80 + cursor_x;



                char display_char = wchar_to_cp866(str[i]);
                buffer[offset] = (attribute << 8) | display_char;

                cursor_x++;
                if (cursor_x >= 80) {
                    cursor_x = 0;
                    cursor_y++;
                    if (cursor_y >= 25) {
                        scroll();
                        cursor_y = 24;
                    }
                }
            }
        }
    }

    void write(int val) {
        char buf[16];
        itoa(val, buf, 10);
        write(buf);
    }

    void writeHex(int val, bool x = true) {
        char buf[16];
        writeHexFromBuffer(val, buf, x);
    }

    void writeHexFromBuffer(int val, char buf[], bool x = true) {
        int_to_hex(val, buf);
        if (x) write("0x");
        write(buf);
    }

    void writeLine(const char *str) {
        write(str);
        write("\n");
    }

    void set_cursor(int x, int y) {
        cursor_x = x;
        cursor_y = y;
        if (cursor_x >= 80) cursor_x = 79;
        if (cursor_y >= 25) cursor_y = 24;
        if (cursor_x < 0) cursor_x = 0;
        if (cursor_y < 0) cursor_y = 0;
    }

    void get_cursor(int &x, int &y) {
        x = cursor_x;
        y = cursor_y;
    }

    char readChar() {  ///for stacktrace

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

    bool hasInput() {  ///for stacktrace

        unsigned char status = inb(0x64);
        return (status & 0x01) != 0;
    }

    string prompt(string message);

    bool isShiftPressed() {  ///for stacktrace
 return shift_pressed; }
    bool isCapsLock() {  ///for stacktrace
 return caps_lock; }
    bool isCtrlPressed() {  ///for stacktrace
 return ctrl_pressed; }
    bool isAltPressed() {  ///for stacktrace
 return alt_pressed; }

};

char Console::scancode_normal[128] = {
        0, 27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
        '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
        0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',
        0, '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0,
        '*', 0, ' ', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, '-', 0, 0, 0, '+', 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

char Console::scancode_shift[128] = {
        0, 27, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', '\b',
        '\t', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n',
        0, 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '"', '~',
        0, '|', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?', 0,
        '*', 0, ' ', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, '-', 0, 0, 0, '+', 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

bool Console::shift_pressed = false;
bool Console::caps_lock = false;
bool Console::ctrl_pressed = false;
bool Console::alt_pressed = false;
unsigned char Console::last_scancode = 0;
bool Console::key_processed = true;

#include "managed/managed.cpp"

string Console::readLine()  {  ///for stacktrace
    uint8_t attribute = get_attribute();
    string res = "";

    int start_x = cursor_x;
    int start_y = cursor_y;
    int current_x = cursor_x;
    int current_y = cursor_y;

    while (true) {
        show_cursor();
        char ch = readChar();
        hide_cursor();

        if (ch == '\n') {
            write("\n");
            return res;
        } else if (ch == '\b') {

            if (res.size() > 0) {
                res.pop_back();


                current_x--;
                if (current_x < 0) {
                    current_x = 79;
                    current_y--;
                    if (current_y < start_y) current_y = start_y;
                }

                buffer[current_y * 80 + current_x] = (attribute << 8) | ' ';

                //buffer[current_y * 80 + current_x] = (0x07 << 8) | ' ';
                set_cursor(current_x, current_y);
            }
        } else if (ch >= 32 && ch <= 126) {

            res += ch;


            buffer[current_y * 80 + current_x] = (attribute << 8) | ch;


            current_x++;
            if (current_x >= 80) {
                current_x = 0;
                current_y++;
                if (current_y >= 25) {

                    scroll();
                    current_y = 24;
                    start_y = (start_y > 0) ? start_y - 1 : 0;
                }
            }

            set_cursor(current_x, current_y);
        }

        else if (ch >= 0x11 && ch <= 0x14) {

            switch (ch) {
                case 0x11:
                    if (current_y > 0) {
                        current_y--;
                        set_cursor(current_x, current_y);
                    }
                    break;
                case 0x12:
                    if (current_y < 24) {
                        current_y++;
                        set_cursor(current_x, current_y);
                    }
                    break;
                case 0x13:
                    if (current_x > 0) {
                        current_x--;
                    } else if (current_y > 0) {
                        current_x = 79;
                        current_y--;
                    }
                    set_cursor(current_x, current_y);
                    break;
                case 0x14:
                    if (current_x < 79) {
                        current_x++;
                    } else if (current_y < 24) {
                        current_x = 0;
                        current_y++;
                    }
                    set_cursor(current_x, current_y);
                    break;
            }
        }
    }
}

string Console::prompt(string message) {
    Console::write(message.data());
    return readLine();
}
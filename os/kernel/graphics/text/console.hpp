#ifndef CONSOLE_H
#define CONSOLE_H


typedef unsigned long long uint64_t;

enum Color {

    BLACK           = 0x000000,
    BLUE            = 0x0000AA,
    GREEN           = 0x00AA00,
    CYAN            = 0x00AAAA,
    RED             = 0xAA0000,
    MAGENTA         = 0xAA00AA,
    BROWN           = 0xAA5500,
    LIGHT_GRAY      = 0xAAAAAA,
    DARK_GRAY       = 0x555555,
    LIGHT_BLUE      = 0x5555FF,
    LIGHT_GREEN     = 0x55FF55,
    LIGHT_CYAN      = 0x55FFFF,
    LIGHT_RED       = 0xFF5555,
    LIGHT_MAGENTA   = 0xFF55FF,
    YELLOW          = 0xFFFF55,
    WHITE           = 0xFFFFFF,


    DARK_BLUE       = 0x000055,
    DARK_GREEN      = 0x005500,
    DARK_CYAN       = 0x005555,
    DARK_RED        = 0x550000,
    DARK_MAGENTA    = 0x550055,
    DARK_BROWN      = 0x552A00,
    DARK_YELLOW     = 0x555500,


    GRAY            = 0x808080,
    DIM_GRAY        = 0x696969,
    SLATE_GRAY      = 0x708090,


    FOREST_GREEN    = 0x228B22,
    NAVY_BLUE       = 0x000080,
    MAROON          = 0x800000,
    PURPLE          = 0x800080,
    OLIVE           = 0x808000,
    TEAL            = 0x008080,


    ORANGE          = 0xFFA500,
    PINK            = 0xFFC0CB,
    GOLD            = 0xFFD700,
    SILVER          = 0xC0C0C0,
    VIOLET          = 0xEE82EE,
    INDIGO          = 0x4B0082,
    CORAL           = 0xFF7F50,
    TURQUOISE       = 0x40E0D0,


    TRANSPARENT     = 0xFFFFFF + 1
};
struct string;

template<typename T> struct managed;

struct ConsoleChar {
    wchar_t symbol;
    Color fg_color;
    Color bg_color;
};

typedef managed<ConsoleChar> CBuffer;

struct Console {
    bool shift_pressed;
    bool ctrl_pressed;
    bool alt_pressed;
    bool caps_lock;

    static const char scancode_normal[128];
    static const char scancode_shift[128];
    static unsigned char last_scancode;
    static bool key_processed;





    Console();

    void init();
    void write(char c, bool both = true);
    void write(const char *s, bool both = true);
    void write(const wchar_t *s, bool both = true);
    void writeLine(const char *s, bool both = true);
    void write(int i);
    void writeHex(int i);
    void write(int i, int base);
    void place(char c);
    void clear();
    void set_color(Color fg, Color bg);
    void set_foreground_color(Color color);
    void set_background_color(Color color);
    void set_font(string name);
    void reset_color();
    bool hasInput();
    void handle_scancode(unsigned char scancode);
    char get_char(unsigned char scancode);
    bool is_arrow_key(unsigned char scancode, int &arrow_type);
    char readChar();
    string readLine();
};

#endif
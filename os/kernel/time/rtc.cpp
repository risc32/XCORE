#pragma once

#include "../types/scalar.cpp"
#include "../managed/managed.cpp"
#include "../stream/stream.cpp"

struct datetime {
    uint8_t second;
    uint8_t minute;
    uint8_t hour;
    uint8_t day;
    uint8_t month;
    uint32_t year;
    uint8_t weekday;

    string getstring() {
        wsstream res;
        res << day << "-" << month << "-" << year << " " << hour << ":" << minute << ":" << second;
        return res.str.data();
    }
};



#include "../cpu/ports.cpp"

static uint8_t read_rtc(uint8_t reg) {
    outb(0x70, reg);
    return inb(0x71);
}

static uint8_t bcd_to_bin(uint8_t bcd) {
    return (bcd >> 4) * 10 + (bcd & 0x0F);
}

datetime get_rtc_time() {
    datetime dt = {0};

    while (read_rtc(0x0A) & 0x80);

    uint8_t statusB = read_rtc(0x0B);
    bool is_bcd = !(statusB & 0x04);
    bool is_24_hour = statusB & 0x02;

    dt.second = read_rtc(0x00);
    dt.minute = read_rtc(0x02);
    dt.hour = read_rtc(0x04);
    dt.weekday = read_rtc(0x06);
    dt.day = read_rtc(0x07);
    dt.month = read_rtc(0x08);
    dt.year = read_rtc(0x09);

    if (is_bcd) {
        dt.second = bcd_to_bin(dt.second);
        dt.minute = bcd_to_bin(dt.minute);
        dt.day = bcd_to_bin(dt.day);
        dt.month = bcd_to_bin(dt.month);
        dt.year = bcd_to_bin(dt.year);

        if (!is_24_hour) {
            bool is_pm = dt.hour & 0x80;
            dt.hour = bcd_to_bin(dt.hour & 0x7F);
            if (is_pm && dt.hour != 12) dt.hour += 12;
            else if (!is_pm && dt.hour == 12) dt.hour = 0;
        } else {
            dt.hour = bcd_to_bin(dt.hour);
        }
    } else {
        if (!is_24_hour) {
            bool is_pm = dt.hour & 0x80;
            dt.hour = dt.hour & 0x7F;
            if (is_pm && dt.hour != 12) dt.hour += 12;
            else if (!is_pm && dt.hour == 12) dt.hour = 0;
        }
    }

    dt.year += 2000;

    return dt;
}

string get_weekday_name(uint8_t weekday) {
    const char* names[] = {"Sunday", "Monday", "Tuesday", "Wednesday",
                           "Thursday", "Friday", "Saturday"};
    return (weekday < 7) ? names[weekday] : "Unknown";
}

string get_month_name(uint8_t month) {
    const char* names[] = {"January", "February", "March", "April", "May", "June",
                           "July", "August", "September", "October", "November", "December"};
    return (month >= 1 && month <= 12) ? names[month-1] : "Unknown";
}
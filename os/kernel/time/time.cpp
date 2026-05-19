#pragma once

#include "rtc.cpp"
#include "measure.cpp"
#include "pit.cpp"
#include "tsc.cpp"

struct Time {
    static void init() {
        tscfreq = get_tsc_freq();
    }
};

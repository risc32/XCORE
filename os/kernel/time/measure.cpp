#pragma once

#include "../utils/utils.cpp"
#include "../CPU/cpu.cpp"


class Timer {
private:
    uint64_t start_ticks;
    uint64_t end_ticks;
    bool running;


    static inline uint64_t rdtsc() {
        uint32_t lo, hi;
        __asm__ __volatile__ (
                "rdtsc"
                : "=a"(lo), "=d"(hi)
                );
        return ((uint64_t)hi << 32) | lo;
    }


    static inline uint64_t rdtscp() {
        uint32_t lo, hi, aux;
        __asm__ __volatile__ (
                "rdtscp"
                : "=a"(lo), "=d"(hi), "=c"(aux)
                );
        return ((uint64_t)hi << 32) | lo;
    }

public:
    Timer() : start_ticks(0), end_ticks(0), running(false) {}


    void start(bool precise = false) {
        if (precise) {

            start_ticks = rdtscp();
        } else {

            start_ticks = rdtsc();
        }
        running = true;
    }


    void stop() {
        if (!running) return;
        end_ticks = rdtsc();
        running = false;
    }

    void reset() {
        start_ticks = 0;
        end_ticks = 0;
        running = false;
    }


    uint64_t elapsed_ticks() const {
        if (running) {
            return rdtsc() - start_ticks;
        }
        return end_ticks - start_ticks;
    }


    double elapsed_ns() const {

        const double cpu_ghz = 2.5;
        return elapsed_ticks() / cpu_ghz;
    }


    double elapsed_us() const {
        return elapsed_ns() / 1000.0;
    }


    double elapsed_ms() const {
        return elapsed_us() / 1000.0;
    }


    double elapsed_seconds() const {
        return elapsed_ms() / 1000.0;
    }


    class ScopedTimer {
    private:
        Timer& timer;
        const char* name;
    public:
        ScopedTimer(Timer& t, const char* desc = nullptr)
                : timer(t), name(desc) {
            timer.start(true);
        }

        ~ScopedTimer() {
            timer.stop();
            if (name) {

            }
        }
    };
};


#define TIME_BLOCK(name) \
    Timer __timer_##__LINE__; \
    Timer::ScopedTimer __scoped_timer_##__LINE__(__timer_##__LINE__, name)

#define TIME_FUNCTION() TIME_BLOCK(__func__)


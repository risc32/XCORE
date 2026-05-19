#pragma once

#include "../utils/utils.cpp"

#define GPI 3.14159265358979323846264338327950288419716939937510582097494459230781640628620899862803482534211706798214808651328230664709384460955058223172535940812848111745028410270193852110555964462294895493038196

namespace Math {
    template<typename T> T generic_pi = GPI;

    long double big_pi = GPI;
    double pi = GPI;
    float fpi = GPI;

    double nan = __builtin_nanf("");
    double inf = __builtin_inff();

    bool isnan(double x) {
        return x != x;
    }

    bool isinf(double x) {

        return (x == __builtin_inf() || x == -__builtin_inf());
    }

    double abs(double x) {
        if (x < 0) return -x;
        return x;
    }

    double pow(double base, int exp) {
        if (exp == 0) return 1;
        if (exp < 0) return 1/base * pow(base, exp+1);
        double ret = base;
        for (int i = 1; i < exp; i++) {
            ret *= base;
        }
        return ret;
    }

    double fact(int x) {
        if (x <= 1) return 1;
        return x * fact(x-1);
    }

    double sqrt(double x) {
        if (x < 0) return -1;
        if (x == 0) return 0;

        double guess = x;
        double prev_guess = 0;
        double epsilon = 1e-10;

        while (abs(guess - prev_guess) > epsilon) {
            prev_guess = guess;
            guess = (guess + x / guess) / 2;
        }

        return guess;
    }

    double fmod(double x, double y) {
        if (y == 0.0 || isinf(x) || isnan(x) || isnan(y)) return nan;
        if (isinf(y)) return x;

        double quotient = (int)(x / y);
        return x - quotient * y;
    }

    double sin(double x) {

        x = fmod(x, 2 * pi);
        if (x > pi) x -= 2 * pi;
        if (x < -pi) x += 2 * pi;

        double ret = 0;
        for (int i = 0; i < 10; i++) {
            ret += (pow(-1, i) * pow(x, 2*i + 1)) / fact(2*i + 1);
        }

        return ret;
    }

    double legend_sin(double x) {
        double ret = 0;
        for (int i = 0; i < 10; i++) {
            ret += (pow(-1, i) * pow(x, 2*i + 1)) / fact(2*i + 1);
        }
        return ret;
    }

    double cos(double x) {

        x = fmod(x, 2 * pi);
        if (x > pi) x -= 2 * pi;
        if (x < -pi) x += 2 * pi;

        double ret = 0;

        for (int i = 0; i < 10; i++) {

            ret += (pow(-1, i) * pow(x, 2 * i)) / fact(2 * i);
        }

        return ret;
    }

    double tan(double x) {return sin(x)/cos(x);}
    double ctg(double x) {return cos(x)/sin(x);}

    double round(double num, uint64_t decimals) {
        double multiplier = pow(10, decimals);
        return (num < 0 ? -1 : 1) * (int64_t)(abs(num) * multiplier + 0.5) / multiplier;
    }

    double sqrt2 = sqrt(2);
}
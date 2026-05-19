#pragma once

int min(int a, int b) {
    return a < b ? a : b;
}

double mind(double a, double b) {
    return a < b ? a : b;
}

int max(int a, int b) {
    return a > b ? a : b;
}

double maxd(double a, double b) {
    return a > b ? a : b;
}

int abs(int a) {
    return a < 0 ? -a : a;
}

int sign(int a) {
    return a < 0 ? -1 : 1;
}
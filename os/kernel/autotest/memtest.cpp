#pragma once

#include "../memory/memory.cpp"
#include "../utils/utils.cpp"
#include "../console.cpp"
#include "restype.cpp"


int memtest(TestResult *results) {
    int test_count = 0;


    results[test_count].test_name = "Basic allocation";
    auto t1 = allocate(64);
    results[test_count++].passed = t1 != nullptr;
    free(t1);

    results[test_count].test_name = "Multiple unique blocks";
    auto t2 = allocate(64);
    auto t3 = allocate(64);
    results[test_count++].passed = t2 != t3 && t2 != nullptr && t3 != nullptr;
    free(t2);
    free(t3);

    results[test_count].test_name = "Zero size returns null";
    auto t4 = allocate(0);
    results[test_count++].passed = t4 == nullptr;
    free(t4);

    results[test_count].test_name = "Memory reuse";
    auto t5 = allocate(64);
    free(t5);
    auto t6 = allocate(64);
    results[test_count++].passed = t5 == t6;
    free(t6);

    results[test_count].test_name = "memcpy basic function";
    auto t7 = (char *) allocate(64);
    auto t8 = (char *) nullptr;
    memcpy(t7, t8, 64);
    bool cmp = true;
    for (int i = 0; i < 64; i++) {
        cmp = cmp && t7[i] == t8[i];
    }
    results[test_count++].passed = cmp;
    free(t7);







    results[test_count].test_name = "Different sizes work";
    auto t11 = allocate(1);
    auto t12 = allocate(512);
    auto t13 = allocate(2048);
    results[test_count++].passed = t11 && t12 && t13;
    free(t11);
    free(t12);
    free(t13);

    results[test_count].test_name = "memcpy data integrity";
    auto t15 = (char *) allocate(32);
    auto t16 = (char *) allocate(32);
    for (int i = 0; i < 32; i++) t15[i] = i;
    memcpy(t16, t15, 32);
    bool copy_ok = true;
    for (int i = 0; i < 32; i++) copy_ok = copy_ok && (t16[i] == i);
    results[test_count++].passed = copy_ok;
    free(t15);
    free(t16);

    results[test_count].test_name = "realloc enlarge with data preservation";
    auto t17 = (char *) allocate(32);
    memset(t17, 0xA, 32);
    auto t18 = (char *) realloc(t17, 64);
    bool realloc_enlarge_ok = (t18 != nullptr) && (t18[0] == 0xA) && (t18[31] == 0xA);
    results[test_count++].passed = realloc_enlarge_ok;
    if (t18) free(t18);

    results[test_count].test_name = "realloc shrink with data preservation";
    auto t19 = (char *) allocate(64);
    memset(t19, 0xB, 64);
    auto t20 = (char *) realloc(t19, 32);
    bool realloc_shrink_ok = (t20 != nullptr) && (t20[0] == 0xB) && (t20[15] == 0xB);
    results[test_count++].passed = realloc_shrink_ok;
    if (t20) free(t20);

    results[test_count].test_name = "memchr finds byte";
    auto t21 = (char *) allocate(50);
    memset(t21, 'A', 50);
    t21[25] = 'X';
    results[test_count++].passed = memchr(t21, 'X', 50) == t21 + 25;
    free(t21);

    results[test_count].test_name = "memchr not found";
    auto t22 = (char *) allocate(50);
    memset(t22, 'A', 50);
    results[test_count++].passed = memchr(t22, 'B', 50) == nullptr;
    free(t22);

    results[test_count].test_name = "memmove overlap right";
    auto t23 = (char *) allocate(50);
    for (int i = 0; i < 50; i++) t23[i] = i;
    memmove(t23 + 10, t23, 20);
    bool move_ok = true;
    for (int i = 0; i < 20; i++) move_ok = move_ok && (t23[10 + i] == i);
    results[test_count++].passed = move_ok;
    free(t23);

    results[test_count].test_name = "memset patterns";
    auto t24 = (char *) allocate(32);
    memset(t24, 0x55, 32);
    bool set_ok = true;
    for (int i = 0; i < 32; i++) set_ok = set_ok && (t24[i] == 0x55);
    results[test_count++].passed = set_ok;
    free(t24);

    results[test_count].test_name = "calloc zeros memory";
    auto t25 = (int *) calloc(10, sizeof(int));
    bool zero_ok = true;
    for (int i = 0; i < 10; i++) zero_ok = zero_ok && (t25[i] == 0);
    results[test_count++].passed = zero_ok;
    free(t25);

    results[test_count].test_name = "Double free safety";
    auto t26 = allocate(16);
    free(t26);
    free(t26);
    results[test_count++].passed = true;

    results[test_count].test_name = "Free nullptr safety";
    free(nullptr);
    results[test_count++].passed = true;

    results[test_count].test_name = "Fragmentation handling";
    auto t27 = allocate(32);
    auto t28 = allocate(32);
    auto t29 = allocate(32);
    free(t28);
    auto t30 = allocate(32);
    results[test_count++].passed = t30 != nullptr;
    free(t27);
    free(t29);
    free(t30);

    results[test_count].test_name = "Boundary access";
    auto t32 = (char *) allocate(8);
    t32[7] = 0xF;
    results[test_count++].passed = t32[7] == 0xF;
    free(t32);

    return test_count;
}
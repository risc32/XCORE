#pragma once

#include "../managed/managed.cpp"
#include "../utils/utils.cpp"
#include "../graphics/text/console.hpp"
#include "restype.cpp"

int mngtest(TestResult *results) {
    int test_count = 0;

    results[test_count].test_name = "Basic int creation and push_back";
    managed<int> int_arr;
    int_arr.push_back(42);
    int_arr.push_back(123);
    results[test_count++].passed = int_arr[0] == 42 && int_arr[1] == 123 && int_arr.size() == 2;

    results[test_count].test_name = "Char insertion at position";
    managed<char> char_arr;
    char_arr.push_back('A');
    char_arr.push_back('C');
    char_arr.insert(1, 'B');
    results[test_count++].passed =
            char_arr[0] == 'A' && char_arr[1] == 'B' && char_arr[2] == 'C' && char_arr.size() == 3;

    results[test_count].test_name = "Double element removal";
    managed<double> double_arr;
    double_arr.push_back(1.1);
    double_arr.push_back(2.2);
    double_arr.push_back(3.3);
    double_arr.remove(1);
    results[test_count++].passed = double_arr[0] == 1.1 && double_arr[1] == 3.3 && double_arr.size() == 2;

    results[test_count].test_name = "Int array subscript operator";
    managed<int> int_arr2;
    int_arr2.push_back(10);
    int_arr2.push_back(20);
    int_arr2[0] = 100;
    int_arr2[1] = 200;
    results[test_count++].passed = int_arr2[0] == 100 && int_arr2[1] == 200;

    results[test_count].test_name = "Float operator+ with single element";
    managed<float> float_arr1;
    float_arr1.push_back(1.5f);
    managed<float> float_arr2 = float_arr1 + 2.5f;
    results[test_count++].passed = float_arr2.size() == 2 && float_arr2[0] == 1.5f && float_arr2[1] == 2.5f;

    results[test_count].test_name = "Int operator+ with another managed";
    managed<int> arr1;
    arr1.push_back(1);
    arr1.push_back(2);
    managed<int> arr2;
    arr2.push_back(3);
    arr2.push_back(4);
    managed<int> arr3 = arr1 + arr2;
    results[test_count++].passed = arr3.size() == 4 && arr3[0] == 1 && arr3[1] == 2 && arr3[2] == 3 && arr3[3] == 4;

    results[test_count].test_name = "Char operator+= with element";
    managed<char> char_arr2;
    char_arr2 += 'X';
    char_arr2 += 'Y';
    results[test_count++].passed = char_arr2.size() == 2 && char_arr2[0] == 'X' && char_arr2[1] == 'Y';

    results[test_count].test_name = "Double operator+= with another managed";
    managed<double> dbl_arr1;
    dbl_arr1.push_back(1.0);
    managed<double> dbl_arr2;
    dbl_arr2.push_back(2.0);
    dbl_arr1 += dbl_arr2;
    results[test_count++].passed = dbl_arr1.size() == 2 && dbl_arr1[0] == 1.0 && dbl_arr1[1] == 2.0;

    results[test_count].test_name = "Int capacity auto-expansion";
    managed<int> expand_arr(2);
    expand_arr.push_back(1);
    expand_arr.push_back(2);
    expand_arr.push_back(3);
    results[test_count++].passed = expand_arr.size() == 3 && expand_arr.capacity() > 2;

    results[test_count].test_name = "Float data() method returns correct pointer";
    managed<float> data_arr;
    data_arr.push_back(1.1f);
    data_arr.push_back(2.2f);
    float *data_ptr = data_arr.data();
    results[test_count++].passed = data_ptr[0] == 1.1f && data_ptr[1] == 2.2f;

    results[test_count].test_name = "Destructor safety";
    {
        managed<int> temp_arr;
        temp_arr.push_back(1);
        temp_arr.push_back(2);
    }
    managed<int> new_arr;
    new_arr.push_back(3);
    results[test_count++].passed = new_arr[0] == 3;

    results[test_count].test_name = "Out of bounds access safety";
    managed<int> bounds_arr;
    bounds_arr.push_back(1);

    results[test_count++].passed = true;
    results[test_count].test_name = "Large number of elements (stress test)";
    managed<int> stress_arr;
    bool stress_ok = true;
    for (int i = 0; i < 100; i++) {
        stress_arr.push_back(i);
        if (stress_arr[i] != i) {
            stress_ok = false;
            break;
        }
    }
    results[test_count++].passed = stress_ok && stress_arr.size() == 100;

    results[test_count].test_name = "Multiple type instances coexistence";
    managed<int> multi_int;
    managed<char> multi_char;
    managed<double> multi_double;

    multi_int.push_back(100);
    multi_char.push_back('Z');
    multi_double.push_back(3.14159);

    results[test_count++].passed = multi_int[0] == 100 && multi_char[0] == 'Z' &&
                                   multi_double[0] == 3.14159;

    results[test_count].test_name = "Initial capacity and size";
    managed<int> init_arr(32);
    results[test_count++].passed = init_arr.size() == 0 && init_arr.capacity() == 32;

    return test_count;

}
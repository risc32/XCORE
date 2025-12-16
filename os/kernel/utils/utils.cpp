#pragma once

#include "itoa.cpp"
#include "../types/types.cpp"


#ifndef stage2
#include "atoi.cpp"
#include "strings.cpp"
#include "basicmath.cpp"
#include "bitwise.cpp"
#include "division64.cpp"
#endif

#define INTEL(x) asm volatile ("\n.intel_syntax noprefix\n" x "\n.att_syntax prefix\n")


#ifndef _INCSCALAR
#define _INCSCALAR

typedef signed char int8_t;
typedef unsigned char uint8_t;

typedef signed short int16_t;
typedef unsigned short uint16_t;

typedef signed int int32_t;
typedef unsigned int uint32_t;

typedef signed long long int64_t;
typedef unsigned long long uint64_t;

typedef int32_t intptr_t;
typedef uint64_t uintptr_t;

typedef decltype(sizeof(0)) size_t;
typedef size_t _size_t;

typedef int32_t ssize_t;

#endif
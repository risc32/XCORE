#pragma once

#include "syscall.cpp"

enum state {
    NEW,
    READY,
    RUNNING,
    BLOCKED,
    DEAD
};

struct process {
    int pid;
    void* stask;
};

struct task {
    int tid;
    state s;
    registers_t registers;
};

struct core {
    
};
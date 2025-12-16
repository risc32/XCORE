#pragma once

#include "../kernel/disk/disk.cpp"
#define OSSEC 4



void install() {
    int bps = disk::driver.geometry.total_bytes/4096;

}
#pragma once

#ifndef size_t
#include <loctypes>
#endif

#include "common/common.hpp"

class xmalloc {
	void* start;
	size_t size;

public:
	xmalloc(void* start, size_t size) {
		setup(start, size);
	}

	void setup(void* start, size_t size) {
		xmalloc::start = start;
		xmalloc::size = size;
	}

	void* malloc(size_t size);
	void free(void* ptr);
};
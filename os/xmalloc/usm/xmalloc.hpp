#ifndef ALLPOOLS

//#define BAREMETAL
#define ALLPOOLS 64

#if not defined(size_t) or defined(BAREMETAL)
#define AUTONOM
#endif

#include "common/common.hpp"
#include "standard/standard.hpp"

namespace xmalloc {
	class allocator {
		void* start;
		size_t size;
		SizeClass* classes[ALLPOOLS];

		void init();
		void distribute();

	public:
		template<typename T, uint64_t size>
		allocator(T (&dedicated)[size]) {
			change((void*)dedicated, sizeof dedicated);
		}

		allocator(void* start, size_t size) {
			setup(start, size);
		}

		void setup(void* start, size_t size) {
			allocator::start = start;
			allocator::size = size;
		}

		void change(void* start, size_t size) {
			setup(start, size);
			init();
		}

		void* malloc(size_t size);
		void free(void* ptr);
	};
}

#include "core/core.hpp"

#endif
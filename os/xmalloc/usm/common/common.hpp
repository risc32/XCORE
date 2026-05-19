#pragma once

#include "panic.cpp"

namespace xmalloc {
    struct SizeClass;
#ifdef AUTONOM
#include <loctypes>
#endif
}

#include "spinlock.cpp"
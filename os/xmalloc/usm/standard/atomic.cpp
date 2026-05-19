#pragma once

namespace xmalloc::detail {

    enum memory_order {
        memory_order_relaxed,
        memory_order_consume,
        memory_order_acquire,
        memory_order_release,
        memory_order_acq_rel,
        memory_order_seq_cst
    };

    template<typename T>
    class atomic {
        volatile T value;
        T load(memory_order order) volatile {
            switch (order) {
                case memory_order_relaxed: return __atomic_load_n(&value, __ATOMIC_RELAXED); break;
                case memory_order_consume: return __atomic_load_n(&value, __ATOMIC_CONSUME); break;
                case memory_order_acquire: return __atomic_load_n(&value, __ATOMIC_ACQUIRE); break;
                case memory_order_release: return __atomic_load_n(&value, __ATOMIC_RELEASE); break;
                case memory_order_acq_rel: return __atomic_load_n(&value, __ATOMIC_ACQ_REL); break;
                case memory_order_seq_cst: return __atomic_load_n(&value, __ATOMIC_SEQ_CST); break;
                default: ;
            }
        }
        void store(T val, memory_order order) volatile {
            switch (order) {
                case memory_order_relaxed: __atomic_store(&value, val, __ATOMIC_RELAXED); break;
                case memory_order_consume: __atomic_store(&value, val, __ATOMIC_CONSUME); break;
                case memory_order_acquire: __atomic_store(&value, val, __ATOMIC_ACQUIRE); break;
                case memory_order_release: __atomic_store(&value, val, __ATOMIC_RELEASE); break;
                case memory_order_acq_rel: __atomic_store(&value, val, __ATOMIC_ACQ_REL); break;
                case memory_order_seq_cst: __atomic_store(&value, val, __ATOMIC_SEQ_CST); break;
                default: ;
            }
        }
        T fetch_add(T val, memory_order order) volatile {
            switch (order) {
                case memory_order_relaxed: return __atomic_fetch_add(&value, val, __ATOMIC_RELAXED);
                case memory_order_consume: return __atomic_fetch_add(&value, val, __ATOMIC_CONSUME);
                case memory_order_acquire: return __atomic_fetch_add(&value, val, __ATOMIC_ACQUIRE);
                case memory_order_release: return __atomic_fetch_add(&value, val, __ATOMIC_RELEASE);
                case memory_order_acq_rel: return __atomic_fetch_add(&value, val, __ATOMIC_ACQ_REL);
                case memory_order_seq_cst: return __atomic_fetch_add(&value, val, __ATOMIC_SEQ_CST);
                default: return 0;
            }
        }
    };
}
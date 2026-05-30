# XCORE

**Experimental operating system kernel built around brutalist design, low-level research and unconventional systems engineering.**

XCORE is a hobby operating system kernel focused on exploring architecture, performance, memory management, custom runtime design and non-standard kernel ideas.

This project is not intended to replicate UNIX, Linux or existing mainstream operating systems.

It is a research playground for building systems from scratch, testing experimental concepts and pushing low-level software design beyond conventional patterns.

---

## Philosophy

XCORE follows a different direction than traditional operating systems.

Core principles:

* **Research over compatibility**
* **Control over abstraction**
* **Custom architecture over inherited standards**
* **Brutalist engineering**
* **Direct low-level design**
* **Experimentation without legacy constraints**

The project intentionally explores solutions that may be unconventional, impractical, aggressive or entirely custom.

---

## Current Features

The kernel currently includes work across multiple low-level subsystems.

### Memory System

* Custom kernel heap allocator
* Protected allocation model
* Alignment-aware memory management
* Heap corruption detection
* Canary validation
* Basic paging infrastructure
* Virtual memory mapping experiments
* Page allocation subsystem

### CPU / Low-Level Core

* Direct hardware interaction
* Register management
* Port I/O abstraction
* CPU instrumentation and tracing
* Function profiling hooks
* Freestanding runtime environment

### Concurrency & Synchronization

* Atomic operations
* Spinlocks
* Mutex implementations
* Timed locking primitives
* Lock guards and scoped synchronization
* Thread-safe internal components

### Runtime & Internal Infrastructure

* Custom memory primitives
* Custom utility layer
* Freestanding string / conversion utilities
* Internal debugging systems
* Lightweight tracing framework
* Exception/runtime handling groundwork

### Kernel Interfaces & Experiments

* Console subsystem
* Text rendering infrastructure
* Input handling foundations
* Custom messaging and internal communication experiments
* Ongoing architectural research

---

## Design Direction

XCORE is heavily shaped by a brutalist aesthetic — both visually and architecturally.

The project favors:

* sharp interfaces
* explicit control
* minimal hidden behavior
* dense low-level implementation
* aggressive ownership of the software stack

Many systems are written manually instead of relying on external runtime assumptions.

If something can be understood, redesigned or rebuilt internally, it probably will be.

---

## Project Status

**Early Development / Experimental**

XCORE is under active development.

Subsystems change frequently.
Architecture is unstable.
Interfaces are not finalized.
Large parts of the design are still being researched, rewritten or replaced.

Expect breaking changes, unfinished components and evolving ideas.

---

## Goals

The long-term direction of XCORE includes research into:

* advanced memory systems
* custom execution models
* kernel-managed graphics concepts
* alternative communication mechanisms
* security and isolation models
* non-standard API architecture
* highly optimized fallback execution paths
* experimental operating system design

---

## Why This Exists

Because operating systems are one of the few places where software engineering, architecture, performance, mathematics and pure experimentation collide.

XCORE exists to explore that space.

---

## Contributing

This is currently a personal research project.

Discussions, ideas, feedback and technical conversations are welcome.

---
# XCore: A Minimalist x86 Operating System Kernel with Memory Management

XCore is a lightweight, bare-metal operating system kernel written in C++ that provides essential memory management, CPU exception handling, and console I/O functionality for x86 architecture. It features a custom memory allocator, CPU exception handling, and a basic console interface, making it suitable for educational purposes and embedded systems development.

The kernel implements core operating system functionality including:
- A custom memory management system with allocation, deallocation, and garbage collection
- CPU exception handling and interrupt management
- Basic console I/O with keyboard input support
- Automated testing framework for kernel components
- Boot loader implementation for x86 architecture
- Memory operations like memcpy, memset, memmove with alignment support
- String manipulation utilities and managed string class

## Repository Structure
```
.
├── CMakeLists.txt          # CMake build configuration
├── comp.bat                # Build and run script for Windows
├── os/                     # Core operating system source code
│   ├── boot.asm           # x86 bootloader implementation
│   └── kernel/            # Kernel source code
│       ├── autotest/      # Automated testing framework
│       ├── cmd/           # Command line interface
│       ├── cpu/           # CPU management and exception handling
│       ├── memory/        # Memory management implementation
│       ├── managed/       # Managed memory and string classes
│       ├── types/         # Core data type definitions
│       └── utils/         # Utility functions
```

## Usage Instructions
### Prerequisites
- i686-elf cross compiler toolchain
- FASM (Flat Assembler) for boot code assembly
- QEMU for system emulation
- CMake 3.28 or higher
- A C++14 compatible compiler

For compilable languages (e.g. C, C++, C#) provide a list of compiler configuation as prerequisites.

### Installation

#### MacOS
```bash
# Install required tools
brew install i686-elf-gcc
brew install fasm
brew install qemu
brew install cmake

# Clone and build
git clone <repository-url>
cd xcore
mkdir build && cd build
cmake ..
make
```

#### Linux/Debian
```bash
# Install required tools
sudo apt-get install i686-elf-gcc
sudo apt-get install fasm
sudo apt-get install qemu-system-x86
sudo apt-get install cmake

# Clone and build
git clone <repository-url>
cd xcore
mkdir build && cd build
cmake ..
make
```

#### Windows
```batch
# Install required tools (using scoop or chocolatey)
scoop install i686-elf-gcc
scoop install fasm
scoop install qemu
scoop install cmake

# Clone and build
git clone <repository-url>
cd xcore
mkdir build
cd build
cmake ..
ninja
```

### Quick Start
1. Build the kernel:
```batch
comp.bat
```

2. Run in QEMU:
```batch
qemu-system-i386 -drive format=raw,file=os.img
```

### More Detailed Examples
1. Memory Management:
```cpp

void* ptr = allocate(64);
memset(ptr, 0, 64);


ptr = realloc(ptr, 128);


free(ptr);
```

2. String Manipulation:
```cpp
string str = "Hello";
str += ", world!";
console.writeLine(str.data());
```

### Troubleshooting
1. Build Fails with "Missing i686-elf-gcc"
- Error: "i686-elf-gcc: command not found"
- Solution: Install the cross-compiler toolchain for your platform
- Verify installation: `i686-elf-gcc --version`

2. QEMU Launch Fails
- Error: "Could not load kernel image"
- Check if os.img was generated successfully
- Verify QEMU installation: `qemu-system-i386 --version`

3. Memory Allocation Failures
- Enable debug mode by defining `DEBUG_MEMORY` in memory.cpp
- Check memory stats using `get_stats()`
- Verify heap size configuration in memory.cpp

## Data Flow
XCore follows a traditional kernel architecture where hardware interactions are abstracted through the CPU and memory management layers.

```ascii
Boot Loader -> Kernel Entry -> Memory Init -> Exception Init -> Console Init
     |            |              |              |                |
     v            v              v              v                v
  [BIOS]    [Protected Mode] [Memory Mgmt] [CPU Exception] [User Interface]
```

Key component interactions:
1. Boot loader initializes system and switches to protected mode
2. Kernel initializes core subsystems (memory, exceptions, console)
3. Memory manager handles allocation requests through memory blocks
4. CPU exceptions are handled through the IDT and custom handlers
5. Console provides I/O interface through memory-mapped display
6. String operations use managed memory for automatic cleanup
7. Automated tests verify system integrity during initialization
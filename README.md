# CharisOS

CharisOS is a lightweight, educational operating system designed for x86_64 architecture. It combines low-level Assembly boot logic with a minimal C runtime, implementing a managed-style kernel architecture that emphasizes simplicity, security, and performance on low-end hardware.

## System Architecture

### Boot Process
The boot sequence begins with `boot/boot.asm`, a Multiboot2-compliant bootloader that handles the transition through three CPU modes:
1. **Real Mode** → GRUB2 places the system in 32-bit protected mode
2. **Protected Mode** → CPU feature detection, page table setup, and GDT initialization
3. **Long Mode** → Final jump to 64-bit code in `boot/long_mode.asm`

The bootloader establishes identity paging using 2MB huge pages for efficient memory management and sets up a 16KB kernel stack.

### Kernel Components

#### Memory Management (`kernel/memory.c`)
- **Physical Memory Manager (PMM)**: Bitmap-based allocator tracking available physical frames
- **Kernel Heap**: Best-fit allocator with split/merge capabilities for dynamic memory allocation
- **Virtual Memory Manager (VMM)**: Foundation for per-process page tables (currently using boot-loaded tables)
- Supports detecting and utilizing available RAM from Multiboot2 memory map

#### Task Management (`kernel/task.c`)
- **Task Abstraction**: Each task has its own kernel stack with guard pages and stack canaries for overflow detection
- **Process Control Block**: Stores state, PID, priority, capabilities, and runtime statistics
- **Maximum 32 concurrent tasks** with 8KB stacks each
- **Preemptive scheduling** with quantum-based time slicing (default 10 ticks)

#### Scheduler (`kernel/scheduler.c`)
- Round-robin scheduling with spinlock-protected task queues
- Task states: READY, RUNNING, BLOCKED, ZOMBIE
- Yield mechanism for cooperative multitasking within preemptive framework

#### Interrupt Handling (`kernel/idt.c`, `kernel/irq.c`)
- 256-entry Interrupt Descriptor Table with interrupt gates
- Full register state capture on exceptions with detailed diagnostics
- Stack canary verification to detect buffer overflows
- PIC remapping to IRQs 32-47 to avoid CPU exceptions conflict

#### Device Drivers
- **VGA Text Driver** (`kernel/vga.c`): 80×25 text mode with color palette, hardware cursor, and scrolling
- **PS/2 Keyboard** (`kernel/keyboard.c`): Scan code to ASCII translation with input buffering
- **PIT Timer** (`kernel/timer.c`): Programmable Interval Timer for system ticks and time measurement
- **RTL8139 Network** (`kernel/net.c`): Basic Ethernet driver for network interface support

#### Security Features
- **Stack Canaries**: Random value placed on stack to detect buffer overflows
- **Guard Pages**: Unmapped memory pages below stacks to catch overruns
- **Capability System**: Fine-grained permission controls (CAP_FS_READ, CAP_FS_WRITE, CAP_SPAWN, etc.)
- **Privilege Separation**: Kernel runs in ring 0 with controlled capability-based access

#### Shell (`kernel/shell.c`)
A built-in command interpreter providing:
- Command parsing with argument support
- Built-in commands: `help`, `clear`, `ls`, `echo`, `net`, `uptime`
- Color-coded output for system messages (info, error, success)

## Features

### Hardware Support
- x86_64 processors with long mode capability (AMD64/Intel 64)
- Minimum 64MB RAM (tested with 256MB)
- VGA-compatible text display
- PS/2 keyboard interface
- RTL8139 network card support (QEMU/emulated environments)

### System Services
- **Multitasking**: Concurrent task execution with preemptive scheduling
- **Memory Management**: Dynamic allocation with heap and physical frame tracking
- **Interrupt Handling**: Hardware and software interrupt processing
- **Timer Services**: Millisecond-precision timing functions
- **Device I/O**: Serial port debugging, VGA output, keyboard input

### Development Features
- **Managed Code Foundation**: Minimal IL (Intermediate Language) runtime support
- **Debugging**: Serial output support, VGA debug logging, exception register dumps
- **Modular Design**: Clear separation between boot, kernel, and application layers

## Technical Specifications

| Component | Implementation |
|-----------|---------------|
| Architecture | x86_64 (64-bit) |
| Boot Standard | Multiboot2 |
| Kernel Language | C (freestanding) |
| Boot Language | NASM Assembly |
| Memory Model | Flat memory model with paging |
| Process Model | Single address space (hybrid kernel design) |
| Maximum Tasks | 32 |
| Default Stack Size | 8KB per task |
| Timer Frequency | Configurable (default 1000 Hz) |

## File Organization

```
├── boot/                 # Bootloader assembly
│   ├── boot.asm          # 32-bit entry, mode transitions
│   └── long_mode.asm     # 64-bit entry point
├── kernel/               # C kernel source
│   ├── main.c            # Kernel entry point
│   ├── memory.c          # PMM, VMM, heap management
│   ├── task.c            # Task creation and management
│   ├── scheduler.c       # Task scheduling
│   ├── idt.c             # Interrupt descriptor table
│   ├── irq.c             # PIC and IRQ handling
│   ├── timer.c           # PIT timer driver
│   ├── keyboard.c        # PS/2 keyboard driver
│   ├── vga.c             # VGA text driver
│   ├── shell.c           # Command shell
│   ├── net.c             # Network driver
│   └── asm/              # Assembly kernel helpers
├── include/kernel/       # Kernel headers
├── iso/                  # ISO image structure
└── build/                # Build artifacts (generated)
```
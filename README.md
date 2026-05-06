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
# Charis OS — Aura-Pi Industrial Edition 🚀
**A specialized fork for Precision Agriculture, Bio-Extraction, and ROS 2 Robotics.**

> [!IMPORTANT]
> This repository is a customized fork of Charis OS, engineered by **Powerdreams Intelligence** for the **Aura-Pi Industrial Node**. It features a hardened kernel and pre-configured drivers specifically for autonomous botanical extraction in Juja, Kenya.

---

## 🛠 Project Aura Customizations
This fork introduces specific enhancements to bridge the gap between low-level hardware and high-level Ag-Tech analytics:
- **I2C Driver Stack:** Native support for the **ADS1115** 16-bit ADC for high-precision soil and substrate pH monitoring.
- **Actuator Control:** Pre-configured PWM logic for **NEMA 17** stepper motors, used in industrial stirring and solvent recovery.
- **Environmental Logic:** Integrated firmware support for DHT22/AM2302 sensors to monitor biomass curing conditions.
- **Aura-Sync:** A background service that bridges ROS 2 telemetry to the [Powerdreams Live Dashboard](https://powerdreams.top).

## 🧠 Core Architecture
- **Base OS:** Ubuntu 24.04 LTS (Minimal Headless Build)
- **Middleware:** ROS 2 Jazzy Jalisco
- **Communication:** MQTT (Secure Bridge) / WebSockets
- **Power Management:** Optimized for solar-buffered Raspberry Pi 5 clusters.

## 🚀 Deployment (Aura-Pi Node)
To initialize an industrial extraction node using this OS:

```bash
# Clone the Aura-Pi fork
git clone [https://github.com/martinmati131-svg/charis__OS.git](https://github.com/martinmati131-svg/charis__OS.git)

# Run the Aura Environment Setup
cd charis__OS
chmod +x setup_aura_node.sh
./setup_aura_node.sh

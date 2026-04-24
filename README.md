# CharisOS

CharisOS is a lightweight educational operating system for x86_64, combining Assembly boot logic with a minimal C runtime and a managed-style kernel architecture.

## Architecture

- `boot/boot.asm` — Multiboot2-aware 32-bit entry and paging / long mode transition.
- `boot/long_mode.asm` — 64-bit entry stub that transfers control to `kernel_main`.
- `kernel/asm/` — Low-level helpers for context switching, GDT flush, and I/O port access.
- `kernel/main.c` — Kernel entrypoint and bootstrap.
- `kernel/*.c` — Core subsystems: VGA, serial, memory, interrupts, scheduler, task management, shell, and minimal IL runtime support.
- `include/kernel/` — Kernel headers.
- `build/link.ld` — Linker layout placing the kernel at 1MiB.
- `iso/boot/grub/grub.cfg` — GRUB boot menu.

## Implemented subsystems

- Bootloader with real mode / protected mode / long mode transition.
- Basic VGA text driver and serial debug output.
- PIC remap, IDT setup, and interrupt dispatch.
- PIT timer and PS/2 keyboard interrupt handlers.
- Simple task abstraction, round-robin scheduler, and cooperative yield.
- Lightweight shell with built-in commands.
- Minimal syscall registration layer.
- Foundation for C# IL runtime integration.

## Build and run

Requirements:
- NASM
- GCC/Clang cross-compiler targeting x86_64 freestanding
- GNU `ld`
- `grub-mkrescue`
- `qemu-system-x86_64`

From the repository root:

```bash
make clean
make all
make run
```

To start in debug mode:

```bash
make debug
```

## Next steps

1. Parse Multiboot2 memory map from `mb_info` and build a real frame allocator.
2. Implement full page table management and virtual memory mapping.
3. Finish the IL loader and interpreter / AOT compiler.
4. Add FAT16 filesystem support and disk driver.
5. Add thread/process isolation and user/kernel protection rings.

## Notes

This project is intentionally minimal and educational. It is designed to bootstrap a real bare-metal OS in a VM and then grow toward a hybrid kernel with managed code support.

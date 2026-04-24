#include <kernel/types.h>
#include <kernel/vga.h>
#include <kernel/serial.h>
#include <kernel/memory.h>
#include <kernel/idt.h>
#include <kernel/irq.h>
#include <kernel/timer.h>
#include <kernel/keyboard.h>
#include <kernel/shell.h>
#include <kernel/syscall.h>
#include <kernel/task.h>
#include <kernel/scheduler.h>
#include <kernel/il_runtime.h>

// Multiboot2 info from bootloader
extern u32 mb_magic;
extern u32 mb_info;

void kernel_main(u32 magic, u32 info) {
    // Initialize VGA text mode
    vga_init();

    // Initialize serial for debugging
    serial_init();

    // Print boot message
    vga_puts("CharisOS booting...\n");
    serial_puts("CharisOS serial debug output\n");

    // Parse multiboot info for memory map
    if (magic != 0x36d76289) {
        vga_puts("ERROR: Invalid multiboot magic!\n");
        serial_puts("ERROR: Invalid multiboot magic!\n");
        while (1);
    }

    // Initialize memory management
    memory_init(info);

    // Set up IDT and ISRs
    idt_init();
    irq_init();

    // Initialize timer (PIT)
    timer_init(1000);

    // Initialize keyboard
    keyboard_init();

    // Initialize scheduler and syscall layer
    task_init();
    scheduler_init();
    syscall_init();

    // Create shell task
    task_t* shell_task = task_create("shell", shell_main, NULL);
    if (shell_task) {
        scheduler_add_task(shell_task);
    }

    shell_init();

    // Enable interrupts
    asm volatile("sti");

    vga_puts("Kernel initialized. Starting shell...\n");

    // Start task scheduler; does not return
    scheduler_start();

    while (1) {
        asm volatile("hlt");
    }
}
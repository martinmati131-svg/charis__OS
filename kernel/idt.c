#include <kernel/idt.h>
#include <kernel/irq.h>
#include <kernel/vga.h>
#include <kernel/serial.h>

#define IDT_ENTRIES 256
static idt_entry_t idt[IDT_ENTRIES];
static idt_ptr_t idt_ptr;

extern u64 isr_table[256];

void idt_init(void) {
    idt_ptr.limit = sizeof(idt_entry_t) * IDT_ENTRIES - 1;
    idt_ptr.base = (u64)&idt;

    // Clear IDT
    for (int i = 0; i < IDT_ENTRIES; i++) {
        idt_set_gate(i, 0, 0, 0);
    }

    // Load ISR stubs from assembly
    for (int i = 0; i < 256; i++) {
        if (isr_table[i]) {
            idt_set_gate(i, isr_table[i], 0, 0x8E); // Interrupt gate
        }
    }

    // Load IDT
    asm volatile("lidt %0" : : "m"(idt_ptr));

    vga_puts("IDT initialized\n");
}

void idt_set_gate(u8 vector, u64 handler, u8 ist, u8 type_attr) {
    idt[vector].offset_low = handler & 0xFFFF;
    idt[vector].selector = 0x08; // Kernel code segment
    idt[vector].ist = ist;
    idt[vector].type_attr = type_attr;
    idt[vector].offset_mid = (handler >> 16) & 0xFFFF;
    idt[vector].offset_high = (handler >> 32) & 0xFFFFFFFF;
    idt[vector].zero = 0;
}

void idt_dispatch_handler(reg_frame_t* frame) {
    if (frame->vector < 32) {
        // Exception
        vga_puts("Exception occurred!\n");
        while (1);
    }

    if (frame->vector >= 32 && frame->vector < 48) {
        u8 irq = frame->vector - 32;
        irq_dispatch(irq, frame);

        if (frame->vector >= 40) {
            asm volatile("outb %0, %1" : : "a"((u8)0x20), "Nd"((u16)0xA0));
        }
        asm volatile("outb %0, %1" : : "a"((u8)0x20), "Nd"((u16)0x20));
    }
}
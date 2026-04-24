#include <kernel/irq.h>
#include <kernel/vga.h>

static void (*irq_handlers[16])(reg_frame_t*) = {0};

void pic_init(void) {
    // ICW1: Start initialization
    asm volatile("outb %0, %1" : : "a"((u8)0x11), "Nd"((u16)PIC1_COMMAND));
    asm volatile("outb %0, %1" : : "a"((u8)0x11), "Nd"((u16)PIC2_COMMAND));

    // ICW2: Remap IRQs to 32-47
    asm volatile("outb %0, %1" : : "a"((u8)IRQ_OFFSET), "Nd"((u16)PIC1_DATA));
    asm volatile("outb %0, %1" : : "a"((u8)(IRQ_OFFSET + 8)), "Nd"((u16)PIC2_DATA));

    // ICW3: Tell master about slave at IRQ2, tell slave its cascade identity
    asm volatile("outb %0, %1" : : "a"((u8)0x04), "Nd"((u16)PIC1_DATA));
    asm volatile("outb %0, %1" : : "a"((u8)0x02), "Nd"((u16)PIC2_DATA));

    // ICW4: 8086 mode
    asm volatile("outb %0, %1" : : "a"((u8)0x01), "Nd"((u16)PIC1_DATA));
    asm volatile("outb %0, %1" : : "a"((u8)0x01), "Nd"((u16)PIC2_DATA));

    // Mask all IRQs initially
    asm volatile("outb %0, %1" : : "a"((u8)0xFF), "Nd"((u16)PIC1_DATA));
    asm volatile("outb %0, %1" : : "a"((u8)0xFF), "Nd"((u16)PIC2_DATA));
}

void pic_send_eoi(u8 irq) {
    if (irq >= 8) {
        asm volatile("outb %0, %1" : : "a"((u8)PIC_EOI), "Nd"((u16)PIC2_COMMAND));
    }
    asm volatile("outb %0, %1" : : "a"((u8)PIC_EOI), "Nd"((u16)PIC1_COMMAND));
}

void pic_mask_irq(u8 irq) {
    u16 port = (irq < 8) ? PIC1_DATA : PIC2_DATA;
    u8 value;
    asm volatile("inb %1, %0" : "=a"(value) : "Nd"(port));
    value |= (1 << (irq % 8));
    asm volatile("outb %0, %1" : : "a"(value), "Nd"(port));
}

void pic_unmask_irq(u8 irq) {
    u16 port = (irq < 8) ? PIC1_DATA : PIC2_DATA;
    u8 value;
    asm volatile("inb %1, %0" : "=a"(value) : "Nd"(port));
    value &= ~(1 << (irq % 8));
    asm volatile("outb %0, %1" : : "a"(value), "Nd"(port));
}

void irq_init(void) {
    pic_init();
    vga_puts("PIC initialized\n");
}

void irq_register_handler(u8 irq, void (*handler)(reg_frame_t*)) {
    irq_handlers[irq] = handler;
    pic_unmask_irq(irq);
}

// Called from idt_dispatch_handler
void irq_dispatch(u8 irq, reg_frame_t* frame) {
    if (irq_handlers[irq]) {
        irq_handlers[irq](frame);
    }
}
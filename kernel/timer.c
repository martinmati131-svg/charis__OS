#include <kernel/timer.h>
#include <kernel/irq.h>
#include <kernel/vga.h>

static u64 timer_ticks = 0;

void timer_init(u32 frequency_hz) {
    u32 divisor = PIT_FREQ / frequency_hz;

    // Command: Channel 0, lobyte/hibyte, rate generator
    asm volatile("outb %0, %1" : : "a"((u8)0x36), "Nd"((u16)PIT_COMMAND));

    // Set divisor
    asm volatile("outb %0, %1" : : "a"((u8)(divisor & 0xFF)), "Nd"((u16)PIT_CHANNEL0));
    asm volatile("outb %0, %1" : : "a"((u8)((divisor >> 8) & 0xFF)), "Nd"((u16)PIT_CHANNEL0));

    // Register IRQ handler
    irq_register_handler(IRQ_TIMER, timer_handler);

    vga_puts("Timer initialized\n");
}

void timer_handler(reg_frame_t* frame) {
    timer_ticks++;
}

u64 timer_get_ticks(void) {
    return timer_ticks;
}

u64 timer_get_ms(void) {
    return timer_ticks * 1000 / (PIT_FREQ / 1000); // Approximate
}

void timer_sleep_ms(u32 ms) {
    u64 start = timer_get_ms();
    while (timer_get_ms() - start < ms) {
        asm volatile("hlt");
    }
}
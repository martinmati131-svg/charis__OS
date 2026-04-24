#include <kernel/serial.h>
#include <kernel/types.h>

#define SERIAL_DATA_PORT(base)          (base)
#define SERIAL_FIFO_COMMAND_PORT(base)  (base + 2)
#define SERIAL_LINE_COMMAND_PORT(base)  (base + 3)
#define SERIAL_MODEM_COMMAND_PORT(base) (base + 4)
#define SERIAL_LINE_STATUS_PORT(base)   (base + 5)

#define SERIAL_LINE_ENABLE_DLAB         0x80

void serial_init(void) {
    u16 port = SERIAL_PORT_COM1;

    // Disable interrupts
    asm volatile("outb %0, %1" : : "a"((u8)0x00), "Nd"(SERIAL_DATA_PORT(port) + 1));

    // Enable DLAB
    asm volatile("outb %0, %1" : : "a"((u8)SERIAL_LINE_ENABLE_DLAB), "Nd"(SERIAL_LINE_COMMAND_PORT(port)));

    // Set divisor to 3 (38400 baud)
    asm volatile("outb %0, %1" : : "a"((u8)0x03), "Nd"(SERIAL_DATA_PORT(port)));
    asm volatile("outb %0, %1" : : "a"((u8)0x00), "Nd"(SERIAL_DATA_PORT(port) + 1));

    // 8 bits, no parity, one stop bit
    asm volatile("outb %0, %1" : : "a"((u8)0x03), "Nd"(SERIAL_LINE_COMMAND_PORT(port)));

    // Enable FIFO, clear them, with 14-byte threshold
    asm volatile("outb %0, %1" : : "a"((u8)0xC7), "Nd"(SERIAL_FIFO_COMMAND_PORT(port)));

    // IRQs enabled, RTS/DSR set
    asm volatile("outb %0, %1" : : "a"((u8)0x0B), "Nd"(SERIAL_MODEM_COMMAND_PORT(port)));
}

static int serial_transmit_empty(u16 port) {
    u8 status;
    asm volatile("inb %1, %0" : "=a"(status) : "Nd"(SERIAL_LINE_STATUS_PORT(port)));
    return status & 0x20;
}

void serial_putchar(char c) {
    u16 port = SERIAL_PORT_COM1;
    while (!serial_transmit_empty(port));
    asm volatile("outb %0, %1" : : "a"(c), "Nd"(SERIAL_DATA_PORT(port)));
}

void serial_write(const char* str) {
    while (*str) {
        serial_putchar(*str++);
    }
}

void serial_writestring(const char* str) {
    serial_write(str);
}

void serial_printf(const char* fmt, ...) {
    // TODO: Implement printf
    serial_write(fmt);
}
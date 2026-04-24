#include <kernel/keyboard.h>
#include <kernel/irq.h>
#include <kernel/vga.h>

static char keyboard_buffer[256];
static u32 keyboard_read_pos = 0;
static u32 keyboard_write_pos = 0;

const char keyboard_scancode_set1[128] = {
    0, 27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
    '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
    0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',
    0, '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0,
    '*', 0, ' ', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '-',
    0, 0, 0, '+', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

const char keyboard_scancode_set1_shift[128] = {
    0, 27, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', '\b',
    '\t', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n',
    0, 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '"', '~',
    0, '|', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?', 0,
    '*', 0, ' ', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '-',
    0, 0, 0, '+', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

void keyboard_init(void) {
    // Register IRQ handler
    irq_register_handler(IRQ_KEYBOARD, keyboard_handler);
    vga_puts("Keyboard initialized\n");
}

void keyboard_handler(reg_frame_t* frame) {
    u8 scancode;
    asm volatile("inb %1, %0" : "=a"(scancode) : "Nd"((u16)KEYBOARD_DATA_PORT));

    if (scancode < 128) {
        char c = keyboard_scancode_set1[scancode];
        if (c) {
            keyboard_buffer[keyboard_write_pos] = c;
            keyboard_write_pos = (keyboard_write_pos + 1) % 256;
        }
    }
}

bool keyboard_get_key(char* out) {
    if (keyboard_read_pos != keyboard_write_pos) {
        *out = keyboard_buffer[keyboard_read_pos];
        keyboard_read_pos = (keyboard_read_pos + 1) % 256;
        return true;
    }
    return false;
}

bool keyboard_has_input(void) {
    return keyboard_read_pos != keyboard_write_pos;
}

void keyboard_read_line(char* buffer, usize max_len) {
    usize i = 0;
    while (i < max_len - 1) {
        char c;
        while (!keyboard_get_key(&c));
        if (c == '\n') break;
        buffer[i++] = c;
        vga_putchar(c);
    }
    buffer[i] = 0;
}
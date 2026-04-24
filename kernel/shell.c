#include <kernel/shell.h>
#include <kernel/vga.h>
#include <kernel/keyboard.h>
#include <kernel/syscall.h>

static void shell_print_prompt(void) {
    vga_puts("charisos> ");
}

void shell_init(void) {
    vga_puts("Shell initialized. Type 'help' for commands.\n");
}

void shell_main(void) {
    char line[128];

    while (1) {
        shell_print_prompt();
        keyboard_read_line(line, sizeof(line));

        if (line[0] == '\0') {
            continue;
        }

        if (line[0] == 'c' && line[1] == 'l' && line[2] == 'e' && line[3] == 'a' && line[4] == 'r') {
            vga_clear();
            continue;
        }
        if (line[0] == 'h' && line[1] == 'e' && line[2] == 'l' && line[3] == 'p') {
            vga_puts("Commands:\n");
            vga_puts("  help  - show this help\n");
            vga_puts("  clear - clear screen\n");
            vga_puts("  ls    - list built-in commands\n");
            vga_puts("  echo  - print text\n");
            continue;
        }
        if (line[0] == 'l' && line[1] == 's') {
            vga_puts("help clear ls echo\n");
            continue;
        }
        if (line[0] == 'e' && line[1] == 'c' && line[2] == 'h' && line[3] == 'o') {
            const char* text = line + 4;
            while (*text == ' ') text++;
            sys_print(text);
            vga_puts("\n");
            continue;
        }

        vga_puts("Unknown command. Type help.\n");
    }
}

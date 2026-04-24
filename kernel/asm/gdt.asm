; gdt.asm
; GDT flush helper for x86_64.
; Called from C after building GDT in memory.

section .text
bits 64
global gdt_flush

; void gdt_flush(gdt_ptr_t* ptr);
gdt_flush:
    lgdt [rdi]
    mov ax, 0x10        ; Kernel data segment
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    ret


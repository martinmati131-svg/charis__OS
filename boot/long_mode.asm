; long_mode.asm
; 64-bit entry point after bootloader sets up paging and GDT.

section .text
bits 64
global long_mode_start
extern kernel_main
extern gdt64

long_mode_start:
    ; Load data segment selectors (GDT offsets: code=0x08, data=0x10)
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    ; Clear unused general-purpose registers (clean state)
    xor rax, rax
    xor rbx, rbx
    xor rcx, rcx
    xor rdx, rdx
    xor rsi, rsi
    xor rdi, rdi
    xor rbp, rbp
    xor r8, r8
    xor r9, r9
    xor r10, r10
    xor r11, r11
    xor r12, r12
    xor r13, r13
    xor r14, r14
    xor r15, r15

    ; Pass multiboot magic and info to kernel_main
    mov edi, dword [mb_magic]
    mov esi, dword [mb_info]

    ; Jump to C kernel
    call kernel_main

    ; If kernel_main returns, halt forever
.halt:
    cli
    hlt
    jmp .halt

section .text
bits 32
; Import mb_magic from boot.asm
extern mb_magic
extern mb_info


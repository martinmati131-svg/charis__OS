; boot.asm
; Multiboot2 compliant bootloader for x86_64 CharisOS
; Transitions: real mode -> protected mode -> long mode
; GRUB2 already puts us in protected mode with A20 enabled.

section .multiboot
align 8
extern long_mode_start
mb_header:
    dd 0xe85250d6                               ; Multiboot2 magic
    dd 0                                        ; Architecture (i386)
    dd mb_header_end - mb_header                ; Header length
    dd 0x100000000 - (0xe85250d6 + 0 + (mb_header_end - mb_header)) ; Checksum

    ; Information request tag (ask for memory map and bootloader name)
    dw 1                                        ; Type
    dw 0                                        ; Flags
    dd 24                                       ; Size
    dd 4                                        ; Memory map
    dd 6                                        ; Memory info
    dd 9                                        ; ELF sections

    ; Address tag (not needed for ELF, but kept for clarity)
    ; dw 2
    ; dw 0
    ; dd 24
    ; dd 0x100000
    ; dd 0x100000
    ; dd 0
    ; dd 0

    ; Entry address tag
    dw 3
    dw 0
    dd 12
    dd start

    ; Flags tag
    dw 4
    dw 0
    dd 8

    ; End tag
    dw 0
    dw 0
    dd 8
mb_header_end:

section .bss
align 4096
; Page tables for identity mapping first 1GB
pml4:
    resb 4096
pdpt:
    resb 4096
pd:
    resb 4096

align 16
stack_bottom:
    resb 32768      ; 32KB stack
stack_top:

section .data
global mb_magic
global mb_info
mb_magic:
    dd 0
mb_info:
    dd 0

section .text
bits 32
global start

start:
    ; GRUB leaves us in protected mode, interrupts disabled.
    ; Set up stack immediately.
    mov esp, stack_top

    ; Save multiboot magic and info pointer
    mov dword [mb_magic], eax
    mov dword [mb_info], ebx

    ; Verify multiboot2 magic
    cmp eax, 0x36d76289
    jne .no_multiboot

    ; Check CPU features
    call check_cpuid
    call check_long_mode

    ; Set up identity paging for first 1GB using 2MB huge pages
    call setup_page_tables

    ; Enable PAE + PGE
    call enable_paging

    ; Load 64-bit GDT
    lgdt [gdt64.pointer]

    ; Far jump to 64-bit code segment
    jmp gdt64.code:long_mode_start

.no_multiboot:
    mov al, "M"
    mov ah, 0x0C        ; Red on black
    mov [0xB8000], ax
    hlt
    jmp .no_multiboot

; ---------------------------------------------------------------------------
; Check CPUID support
; ---------------------------------------------------------------------------
check_cpuid:
    pushfd
    pop eax
    mov ecx, eax
    xor eax, 1 << 21
    push eax
    popfd
    pushfd
    pop eax
    xor eax, ecx
    jz .no_cpuid
    ret
.no_cpuid:
    mov al, "C"
    mov ah, 0x0C
    mov [0xB8000], ax
    hlt
    jmp .no_cpuid

; ---------------------------------------------------------------------------
; Check long mode support (extended processor info)
; ---------------------------------------------------------------------------
check_long_mode:
    mov eax, 0x80000000
    cpuid
    cmp eax, 0x80000001
    jb .no_long_mode

    mov eax, 0x80000001
    cpuid
    test edx, 1 << 29
    jz .no_long_mode
    ret

.no_long_mode:
    mov al, "L"
    mov ah, 0x0C
    mov [0xB8000], ax
    hlt
    jmp .no_long_mode

; ---------------------------------------------------------------------------
; Set up page tables: identity map first 1GB with 2MB huge pages
; ---------------------------------------------------------------------------
setup_page_tables:
    ; Clear page tables
    mov edi, pml4
    xor eax, eax
    mov ecx, 4096 * 3 / 4   ; 3 pages = 12288 bytes, /4 for dwords
    rep stosd

    ; PML4[0] -> PDPT
    mov edi, pml4
    mov eax, pdpt
    or eax, 0x03            ; Present + Writable
    mov [edi], eax

    ; PDPT[0] -> PD
    mov edi, pdpt
    mov eax, pd
    or eax, 0x03
    mov [edi], eax

    ; PD[i] = (i * 2MB) | HugePage | Present | Writable
    mov edi, pd
    mov eax, 0x83           ; Present + Writable + Huge
    mov ecx, 512            ; 512 entries = 1GB
.pt_loop:
    mov [edi], eax
    add eax, 0x200000       ; Next 2MB page
    add edi, 8
    loop .pt_loop

    ret

; ---------------------------------------------------------------------------
; Enable PAE, long mode, and paging
; ---------------------------------------------------------------------------
enable_paging:
    ; Load PML4 address into CR3
    mov eax, pml4
    mov cr3, eax

    ; Enable PAE (Physical Address Extension)
    mov eax, cr4
    or eax, 1 << 5          ; PAE bit
    mov cr4, eax

    ; Enable long mode (LME) via EFER MSR
    mov ecx, 0xC0000080     ; EFER MSR
    rdmsr
    or eax, 1 << 8          ; LME bit
    wrmsr

    ; Enable paging
    mov eax, cr0
    or eax, 1 << 31         ; PG bit
    mov cr0, eax

    ret

; ---------------------------------------------------------------------------
; 64-bit GDT (flat model)
; ---------------------------------------------------------------------------
section .rodata
gdt64:
    dq 0                        ; Null descriptor
.code: equ $ - gdt64
    dq (1 << 43) | (1 << 44) | (1 << 47) | (1 << 53) ; 64-bit code, ring 0
.data: equ $ - gdt64
    dq (1 << 44) | (1 << 47)    ; Data, ring 0
.user_code: equ $ - gdt64
    dq (1 << 43) | (1 << 44) | (1 << 46) | (1 << 47) | (1 << 53) ; 64-bit code, ring 3
.user_data: equ $ - gdt64
    dq (1 << 44) | (1 << 46) | (1 << 47) ; Data, ring 3
.pointer:
    dw $ - gdt64 - 1
    dq gdt64


/* idt.h - Interrupt Descriptor Table */
#pragma once
#include <kernel/types.h>

/* Register frame pushed by ISR stubs */
typedef struct {
    u64 r15, r14, r13, r12, r11, r10, r9, r8;
    u64 rdi, rsi, rbp, rbx, rdx, rcx, rax;
    u64 vector;
    u64 error_code;
    u64 rip, cs, rflags, rsp, ss;
} PACKED reg_frame_t;

/* IDT entry (64-bit) */
typedef struct {
    u16 offset_low;
    u16 selector;
    u8  ist;
    u8  type_attr;
    u16 offset_mid;
    u32 offset_high;
    u32 zero;
} PACKED idt_entry_t;

/* IDT pointer for lidt */
typedef struct {
    u16 limit;
    u64 base;
} PACKED idt_ptr_t;

void idt_init(void);
void idt_set_gate(u8 vector, u64 handler, u8 ist, u8 type_attr);
void idt_dispatch_handler(reg_frame_t* frame);

/* ISR table exported from assembly */
extern u64 isr_table[256];


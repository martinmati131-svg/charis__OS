/* il_runtime.h - Minimal C# runtime headers */
#pragma once
#include <kernel/types.h>

/* Object header for every managed object */
typedef struct {
    u32 vtable_idx;         /* Index into vtable array */
    u32 flags;              /* GC flags, etc. */
    u32 size;               /* Object size in bytes */
    u32 ref_count;          /* For reference counting (optional) */
} il_object_header_t;

#define IL_FLAG_REACHABLE   0x01
#define IL_FLAG_PINNED      0x02
#define IL_FLAG_FINALIZED   0x04

/* VTable entry */
typedef struct {
    const char* type_name;
    u32 field_count;
    u32 method_count;
    u32 static_size;
    void** methods;         /* Array of function pointers or IL offsets */
    u32* field_offsets;
} il_vtable_t;

/* Method metadata */
typedef struct {
    const char* name;
    u32 il_offset;          /* Offset into IL blob */
    u32 il_size;
    u32 locals_size;        /* Size of local variable space */
    u32 max_stack;
    u32 arg_count;
    u32 ret_type;           /* 0=void, 1=int32, 2=int64, 3=ptr, 4=obj */
} il_method_t;

/* Static field storage */
typedef struct {
    u32 vtable_idx;
    u32 field_idx;
    u64 value;
} il_static_field_t;

/* Execution frame */
typedef struct {
    u64* stack;
    u64* locals;
    u64* args;
    u32 stack_pos;
    u32 stack_size;
    il_method_t* method;
} il_frame_t;

/* IL interpreter state */
typedef struct {
    u8* il_blob;
    u32 il_size;
    il_vtable_t* vtables;
    u32 vtable_count;
    il_method_t* methods;
    u32 method_count;
    il_static_field_t* statics;
    u32 static_count;
} il_image_t;

/* Runtime API */
void il_runtime_init(il_image_t* image);
void il_runtime_exec(const char* entry_method);
void* il_alloc_obj(u32 vtable_idx);
void* il_alloc_array(u32 elem_size, u32 count);
void il_gc_collect(void);

/* Runtime-native trampolines exposed to interpreted code */
u64 il_native_call(u32 native_id, u64* args);


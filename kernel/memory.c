#include <kernel/memory.h>
#include <kernel/vga.h>
#include <kernel/serial.h>

// Multiboot info pointer
extern u32 mb_info;

// Physical memory bitmap
static u8* pmm_bitmap = NULL;
static u64 pmm_bitmap_size = 0;
u64 pmm_total_frames = 0;
u64 pmm_used_frames = 0;
static u64 pmm_base_addr = 0;

// Kernel heap
static void* heap_start = NULL;
static usize heap_size = 0;
static void* heap_current = NULL;

void memory_init(u32 info) {
    // Parse multiboot memory map
    // TODO: Implement proper parsing

    // For now, assume 128MB RAM starting at 1MB
    u64 total_mem = 128 * 1024 * 1024;
    pmm_total_frames = total_mem / PAGE_SIZE;
    pmm_bitmap_size = (pmm_total_frames + 7) / 8; // Bits to bytes
    pmm_base_addr = 1 * 1024 * 1024; // Start after bootloader

    // Allocate bitmap in kernel space (assume it's already mapped)
    pmm_bitmap = (u8*)0x100000; // 1MB mark

    // Clear bitmap
    for (u64 i = 0; i < pmm_bitmap_size; i++) {
        pmm_bitmap[i] = 0;
    }

    // Mark first 2MB as used (bootloader, kernel)
    for (u64 i = 0; i < (2 * 1024 * 1024) / PAGE_SIZE; i++) {
        pmm_bitmap[i / 8] |= (1 << (i % 8));
        pmm_used_frames++;
    }

    vga_puts("PMM initialized\n");

    // Initialize heap
    heap_start = (void*)0x200000; // 2MB
    heap_size = 64 * 1024 * 1024; // 64MB heap
    heap_current = heap_start;

    vga_puts("Heap initialized\n");

    // Initialize VMM
    vmm_init();
    vga_puts("VMM initialized\n");
}

void* pmm_alloc_frame(void) {
    for (u64 i = 0; i < pmm_total_frames; i++) {
        u64 byte = i / 8;
        u8 bit = i % 8;
        if ((pmm_bitmap[byte] & (1 << bit)) == 0) {
            pmm_bitmap[byte] |= (1 << bit);
            pmm_used_frames++;
            return (void*)(pmm_base_addr + i * PAGE_SIZE);
        }
    }
    return NULL; // Out of memory
}

void pmm_free_frame(void* addr) {
    u64 frame = ((u64)addr - pmm_base_addr) / PAGE_SIZE;
    u64 byte = frame / 8;
    u8 bit = frame % 8;
    if (pmm_bitmap[byte] & (1 << bit)) {
        pmm_bitmap[byte] &= ~(1 << bit);
        pmm_used_frames--;
    }
}

void heap_init(void* start, usize size) {
    heap_start = start;
    heap_size = size;
    heap_current = start;
}

void* kmalloc(usize size) {
    if ((usize)heap_current + size > (usize)heap_start + heap_size) {
        return NULL; // Out of heap
    }
    void* ptr = heap_current;
    heap_current = (void*)((usize)heap_current + size);
    return ptr;
}

void* krealloc(void* ptr, usize size) {
    // Simple realloc - just alloc new and copy
    void* new_ptr = kmalloc(size);
    if (new_ptr && ptr) {
        // Copy old data (assume size is known somehow)
    }
    return new_ptr;
}

void kfree(void* ptr) {
    // No-op for now - simple bump allocator
}

void vmm_init(void) {
    // Already set up by bootloader
}

void* vmm_alloc_page(void) {
    void* phys = pmm_alloc_frame();
    if (!phys) return NULL;
    // Map to some virtual address - TODO
    return phys; // For now
}

void vmm_free_page(void* virt) {
    void* phys = vmm_get_phys(virt);
    pmm_free_frame(phys);
    vmm_unmap_page(virt);
}

void vmm_map_page(void* phys, void* virt, u64 flags) {
    // TODO: Implement page table manipulation
}

void vmm_unmap_page(void* virt) {
    // TODO
}

void* vmm_get_phys(void* virt) {
    // TODO: Walk page tables
    return virt; // Identity mapped for now
}
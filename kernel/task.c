#include <kernel/task.h>
#include <kernel/memory.h>
#include <kernel/vga.h>
#include <kernel/scheduler.h>

static task_t task_pool[TASK_MAX_TASKS];
static task_t* task_list = NULL;
static task_t* current_task = NULL;
static u32 next_pid = 1;

void task_init(void) {
    for (u32 i = 0; i < TASK_MAX_TASKS; i++) {
        task_pool[i].rsp = 0;
        task_pool[i].stack_base = 0;
        task_pool[i].state = TASK_STATE_ZOMBIE;
        task_pool[i].pid = 0;
        task_pool[i].priority = 0;
        task_pool[i].next = NULL;
        task_pool[i].prev = NULL;
        task_pool[i].runtime_ticks = 0;
    }
    task_list = NULL;
    current_task = NULL;
}

static task_t* task_allocate(void) {
    for (u32 i = 0; i < TASK_MAX_TASKS; i++) {
        if (task_pool[i].state == TASK_STATE_ZOMBIE) {
            return &task_pool[i];
        }
    }
    return NULL;
}

void task_exit_handler(void) {
    if (current_task) {
        current_task->state = TASK_STATE_ZOMBIE;
        current_task->runtime_ticks = 0;
    }
    scheduler_yield();
    while (1) {
        asm volatile("hlt");
    }
}

void task_exit(void) {
    if (current_task) {
        current_task->state = TASK_STATE_ZOMBIE;
    }
    scheduler_yield();
}

void task_block(task_t* task) {
    if (task) {
        task->state = TASK_STATE_BLOCKED;
    }
}

void task_unblock(task_t* task) {
    if (task && task->state == TASK_STATE_BLOCKED) {
        task->state = TASK_STATE_READY;
    }
}

task_t* task_current(void) {
    return current_task;
}

task_t* task_create(const char* name, task_func_t func, void* arg) {
    task_t* task = task_allocate();
    if (!task) {
        vga_puts("Failed to allocate task\n");
        return NULL;
    }

    void* stack = kmalloc(TASK_STACK_SIZE);
    if (!stack) {
        vga_puts("Failed to allocate task stack\n");
        return NULL;
    }

    task->stack_base = (u64)stack;
    task->pid = next_pid++;
    task->state = TASK_STATE_READY;
    task->priority = 0;
    task->runtime_ticks = 0;
    for (u32 i = 0; i < TASK_NAME_MAX - 1 && name && name[i]; i++) {
        task->name[i] = name[i];
    }
    task->name[TASK_NAME_MAX - 1] = '\0';

    u64* stack_top = (u64*)((u8*)stack + TASK_STACK_SIZE);
    stack_top = (u64*)((u64)stack_top & ~0xFUL);

    *--stack_top = (u64)arg;             // Argument for task function
    *--stack_top = (u64)func;            // Entry point for task_trampoline
    *--stack_top = (u64)task_trampoline; // Return address for initial context
    *--stack_top = 0;                    // rbp
    *--stack_top = 0;                    // rbx
    *--stack_top = 0;                    // r12
    *--stack_top = 0;                    // r13
    *--stack_top = 0;                    // r14
    *--stack_top = 0;                    // r15

    task->rsp = (u64)stack_top;
    task->next = NULL;
    task->prev = NULL;

    return task;
}

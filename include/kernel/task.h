/* task.h - Process and thread management */
#pragma once
#include <kernel/types.h>
#include <kernel/idt.h>

#define TASK_STATE_READY        0
#define TASK_STATE_RUNNING      1
#define TASK_STATE_BLOCKED      2
#define TASK_STATE_ZOMBIE       3

#define TASK_NAME_MAX           32
#define TASK_STACK_SIZE         32768
#define TASK_MAX_TASKS          32

typedef struct task {
    u64 rsp;                        /* Kernel stack pointer */
    u64 stack_base;                 /* Base of allocated stack */
    u32 state;
    u32 pid;
    u32 priority;
    char name[TASK_NAME_MAX];
    struct task* next;
    struct task* prev;
    u64 runtime_ticks;
} task_t;

typedef void (*task_func_t)(void* arg);

void scheduler_init(void);
void scheduler_tick(reg_frame_t* frame);
void scheduler_add_task(task_t* task);
void scheduler_remove_task(task_t* task);
task_t* scheduler_current(void);
void scheduler_yield(void);

void task_init(void);
task_t* task_create(const char* name, task_func_t func, void* arg);
void task_exit(void);
void task_block(task_t* task);
void task_unblock(task_t* task);

/* Assembly context switch */
void context_switch(u64* old_rsp, u64 new_rsp);
void task_trampoline(void);

/* Task exit handler called from assembly */
void task_exit_handler(void);


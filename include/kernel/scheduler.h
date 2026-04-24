/* scheduler.h - Cooperative round-robin scheduler */
#pragma once
#include <kernel/types.h>
#include <kernel/task.h>
#include <kernel/idt.h>

void scheduler_init(void);
void scheduler_add_task(task_t* task);
void scheduler_start(void);
void scheduler_schedule(void);
void scheduler_yield(void);

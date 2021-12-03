#ifndef _SCHEDULER_H_
#define _SCHEDULER_H_

#include <multitasking/schedlock.h>
#include <multitasking/task.h>

void mt_init();
void mt_panic();

uint8_t mt_is_initialized();
task_t *mt_get_current_task();

void mt_set_fg_task(task_t *task);
task_t *mt_get_fg_task();

task_t *mt_get_task_by_pid(uint32_t pid);

uint32_t mt_spawn_ktask(char *name, uint32_t ppid, void (*entry)(void *),
                        void *args);
uint32_t mt_spawn_utask(char *name, uint32_t ppid, char *path, void *args);

void mt_task_terminate(task_t *t);
void mt_task_terminate_pid(uint32_t pid);

void mt_switch_task(task_t*);
void mt_schedule(void);

/* TODO : Delete these when finished testing */
void mt_print_tasks();
void rush(void *x);

#endif

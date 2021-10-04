#ifndef _TASK_H_
#define _TASK_H_

#include <stdint.h>
#include <ds/list.h>

/* Task states */
#define TS_SUS 0
#define TS_RDY 1
#define TS_RUN 2
#define TS_FIN 3

#define DEFAULT_TIME_SLICE 1
#define DEFAULT_STACK_SIZE (0x4000)

#define MAX_PID 0xFFFF

/* Note : I might add more members to this struct */
typedef struct task {
  uint32_t esp;
  uint32_t pid;
  uint32_t uid;

  uint32_t esp0;
  uint32_t cr3;
  uint32_t allocated_stack;
  uint32_t stack_size;

  uint32_t time_slice;
  uint32_t time_remaining;

  uint32_t ppid;
  struct task *next;
  struct task *prev;
  
  list_t *file_descriptors;
  
  uint8_t state;
  char name[128];
} task_t;

task_t *ktask_create(char *name, uint32_t ppid, void (*entry)(void *),
                     void *args);
task_t *utask_create(char *name, uint32_t ppid, char *path, void *args);

void task_destroy(task_t *task);
void utask_exec(void *args);

#endif

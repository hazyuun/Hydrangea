#include <multitasking/task.h>
#include <multitasking/scheduler.h>

#include <util/logger.h>

#include <mem/heap.h>
#include <mem/paging.h>

#include <stddef.h>
#include <string.h>

static uint32_t gen_pid(){
  static uint32_t _pid = 0;
  if(_pid > MAX_PID){
    _pid = 0;
    while(mt_get_task_by_pid(_pid)){
      _pid++;
    }
  }
  return _pid++;  
}

/* Creates a kernel task from a routine */
task_t *ktask_create(char *name, uint32_t ppid, void (*entry)(void*), void *args){

  /* Verify if the parent exists before allocating a task_t structure */
  /* in order to avoid calling kmalloc then kfree shortly after */

  uint32_t pid = gen_pid();    
  if(pid && !mt_get_task_by_pid(ppid)){
    log_f(ERROR, "ktask_create", "Parent task (PID %d) doesn't exist", ppid);
    return NULL;
  }

  task_t *task = (task_t *) kmalloc(sizeof(task_t));
  strcpy(task->name, name);
  task->pid = pid;
  task->ppid = ppid;

  task->state = TS_RDY;
  task->stack_size = DEFAULT_STACK_SIZE;

  /* Setting up the stack */
  uint8_t *s = (uint8_t *) kmalloc(task->stack_size);
  uint32_t *stack = (uint32_t *)(s + task->stack_size - 4);
  
  *(stack--) = (uint32_t) args; /* void *args */
  *(stack--) = 0;
  *(stack--) = (uint32_t) entry; /* Entry point */
  *(stack--) = 0; /* eax */
  *(stack--) = 0; /* ebx */
  *(stack--) = 0; /* esi */
  *(stack--) = 0; /* edi */
  *(stack) = 0;   /* ebp */

  task->next = NULL;

  task->esp = (uint32_t) stack;
  task->allocated_stack = (uint32_t) s;
  task->cr3 = (uint32_t) pg_get_ker_dir();

  task->time_slice = DEFAULT_TIME_SLICE;
  task->time_remaining = DEFAULT_TIME_SLICE;
  log_f(INFO, "ktask_create", "Created kernel task (PID %d) : %s", task->pid, name);
  
  return task;
}

void task_destroy(task_t *task){
  kfree((void*)(task->allocated_stack));
  kfree(task);
}

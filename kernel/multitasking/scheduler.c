#include <multitasking/schedlock.h>
#include <multitasking/scheduler.h>

#include <mem/heap.h>
#include <mem/paging.h>

#include <util/logger.h>
#include <term/term.h>

#include <stdio.h>
#include <string.h>

uint8_t _mt_enabled = 0;

/* A linked list containing all task control blocks */
task_t *tasks = 0;
uint32_t tasks_count;

/* The currently running task */
task_t *cur_task;

/* Those will be deleted by the cleaner task */
task_t *terminated_tasks = NULL;

inline uint8_t mt_is_initialized() { return _mt_enabled; }

static void mt_push_task(task_t *new_task) {
  if (!tasks) {
    tasks = new_task;
    return;
  }

#if 0
  new_task->next = tasks;
  tasks->prev = new_task;
  tasks = new_task;

#else
  task_t *t = tasks;
  while (t->next) {
    t = t->next;
  }
  t->next = new_task;
  new_task->prev = t;
#endif
}

void ker_idle(void *args);
void cleaner(void*);

void mt_init() {
  log_info(NICE_MAGENTA, "SCHED", "Initializing multitasking");
  
  task_t *kernel_idle = ktask_create("kernel_idle", 0, &ker_idle, NULL);
  mt_push_task(kernel_idle);

  mt_push_task(ktask_create("test", 0, &ker_idle, (void *)0));
  mt_push_task(ktask_create("cleaner", 0, &cleaner, (void *)0));
  
  
  cur_task = tasks;
  _mt_enabled = 1;
}

inline void mt_panic() { _mt_enabled = 0; }

uint32_t mt_spawn_ktask(char *name, uint32_t ppid, void (*entry)(void *),
                        void *args) {
  task_t *t = ktask_create(name, ppid, entry, args);
  mt_push_task(t);
  return t->pid;
}

uint32_t mt_spawn_utask(char *name, uint32_t ppid, char *path, void *args) {
  task_t *t = utask_create(name, ppid, path, args);
  if(!t) return 0;
  mt_push_task(t);
  return t->pid;
}

inline task_t *mt_get_current_task() { return cur_task; }

task_t *mt_get_task_by_pid(uint32_t pid) {
  task_t *t = tasks;

  while (t && t->pid != pid) {
    t = t->next;
  }

  return t;
}

static task_t *mt_pick_next_task() {
  task_t *next_task = cur_task->next;
  if (!next_task)
    return tasks;  
  
  if(next_task->state != TS_FIN)
    return next_task;

  next_task = next_task->next;

  while (next_task) {
    if(next_task->state != TS_FIN)
      return next_task;
    next_task = next_task->next;
  }

  return tasks;
}


#include <drivers/serial.h>
void cleaner(void *args){
  (void)args;
  while (1) {
    uint32_t ef = get_eflags_and_cli();

    task_t *t = terminated_tasks;
    if(!t){
      set_eflags_and_sti(ef);
      continue;
    }

    while (t) {
      task_t *next = t->next;
      
      log_info(NICE_CYAN, "cleaner", "removing PID %d", t->pid);
      task_destroy(t);
      t = next;
    }

    terminated_tasks = NULL;

    set_eflags_and_sti(ef);
  }
} 

void mt_task_terminate(task_t *t) {
  if(t->pid == 0)
    return log_f(ERROR, "mt_task_terminate", "PID 0 can't be terminated");

  if(t->prev)
    t->prev->next = t->next;
  
  if(t->next)
    t->next->prev = t->prev;

  
  if (!terminated_tasks) {
    terminated_tasks = t;
    t->next = NULL;
    t->prev = NULL;
  } else {
    t->next = terminated_tasks;
    terminated_tasks->prev = t;
    terminated_tasks = t;
  }
  
  task_t *i = tasks;
  while (i) {
    if (i->ppid == t->pid)
      mt_task_terminate(i);
    
    i = i->next;
  }
}

void mt_task_terminate_pid(uint32_t pid) {
  task_t *t = mt_get_task_by_pid(pid);
  if(!t) return log_f(ERROR, "mt_task_terminate_pid", "PID %d does not exist", pid);
  mt_task_terminate(t);
}

void mt_task_self_terminate() { mt_task_terminate(cur_task); }

#include <cpu/tss.h>

void mt_schedule(void) {
  if (!mt_is_initialized() || tasks_count == 1)
    return;

  cur_task->time_remaining -= 1;
  if(cur_task->state == TS_RUN && cur_task->time_remaining > 0){ 
    return;
  }
  cur_task->time_remaining = cur_task->time_slice;
  
  task_t *next_task = mt_pick_next_task();
  
  tss_set_esp0(next_task->allocated_stack + DEFAULT_STACK_SIZE);
  if(cur_task->cr3 != next_task->cr3){
    pg_switch_page_dir(next_task->cr3);
    pg_invalidate_cache();
  }
  
  next_task->state = TS_RUN;
  if(cur_task->state != TS_FIN)
    cur_task->state = TS_SUS;
  
  mt_switch_task(next_task);
}

/* Just for debugging */
void mt_print_tasks() {
  task_t *t = tasks;
  printk("\n PID | PPID \t| name");
  printk("\n --- + -------- + ----");
  while (t) {
    printk("\n %d \t | %d \t\t| %s\t\t | %d", t->pid, t->ppid, t->name, t->state);
    t = t->next;
  }
}
void mt_print_terminated_tasks() {
  task_t *t = terminated_tasks;
  printk("\n PID | PPID \t| name");
  printk("\n --- + -------- + ----");
  while (t) {
    printk("\n %d \t | %d \t\t| %s\t\t | %d", t->pid, t->ppid, t->name, t->state);
    t = t->next;
  }
}

void ker_idle(void *args) {
  (void)args;
  while (1) {
    __asm__ __volatile__("hlt");
  }
}

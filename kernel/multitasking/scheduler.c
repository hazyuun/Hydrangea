#include <multitasking/schedlock.h>
#include <multitasking/scheduler.h>

#include <mem/heap.h>
#include <mem/paging.h>

#include <util/logger.h>

#include <stdio.h>
#include <string.h>

uint8_t _mt_enabled = 0;

/* A linked list containing all task control blocks */
task_t *tasks = 0;
uint32_t tasks_count;

/* The currently running task */
task_t *cur_task;

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

/* To test usermode later */
void gpf(void *args);

/* TODO: Make a proper ELF loader and delete this shit */
void user_main(void){
  while(1){
    __asm__ volatile("int $0x80");
  }
}

#include <term/term.h>
void mt_init() {
  log_info(NICE_MAGENTA, "SCHED", "Initializing multitasking");
  
  task_t *kernel_idle = ktask_create("kernel_idle", 0, &ker_idle, NULL);
  mt_push_task(kernel_idle);

  mt_push_task(ktask_create("test", 0, &ker_idle, (void *)0));
  //mt_push_task(ktask_create("user", 0, &utask_exec, (void *)&user_main));

  cur_task = tasks;
  _mt_enabled = 1;
}

void mt_panic() { _mt_enabled = 0; }

uint32_t mt_spawn_ktask(char *name, uint32_t ppid, void (*entry)(void *),
                        void *args) {
  task_t *t = ktask_create(name, ppid, entry, args);
  mt_push_task(t);
  return t->pid;
}

uint32_t mt_spawn_utask(char *name, uint32_t ppid, void (*entry)(void *),
                        void *args) {
  task_t *t = utask_create(name, ppid, entry, args);
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
  return next_task;
}

static void mt_clean_finished_tasks() {

  uint32_t new_count = tasks_count;
  task_t *t = tasks;

  while (t) {
    task_t *next = t->next;

    if (t->state == TS_FIN) {

      t->prev->next = t->next;
      t->next->prev = t->prev;
      task_destroy(t);

      --new_count;
    }
    t = next;
  }
  
  tasks_count = new_count;
}


void mt_task_terminate(task_t *t) {
  if(t->pid == 0)
    return log_f(ERROR, "mt_task_terminate", "PID 0 can't be terminated");

  t->state = TS_FIN;

  /* TODO: This is slow */
  task_t *i = tasks;
  while (i) {
    if (i->ppid == t->pid) {
      i->state = TS_FIN;
    }
    i = i->next;
  }
}

void mt_task_terminate_pid(uint32_t pid) {
  task_t *t = mt_get_task_by_pid(pid);
  mt_task_terminate(t);
}

void mt_task_self_terminate() { mt_task_terminate(cur_task); }

#include <cpu/tss.h>

void mt_schedule(void) {
  if (!mt_is_initialized() || tasks_count == 1)
    return;

  mt_clean_finished_tasks();
  
  cur_task->time_remaining -= 1;
  if(cur_task->state == TS_RUN && cur_task->time_remaining > 0){ 
    return;
  }
  cur_task->time_remaining = cur_task->time_slice;
  

  cur_task->state = TS_SUS;

  task_t *next_task = mt_pick_next_task();
  
  tss_set_esp0(next_task->allocated_stack + DEFAULT_STACK_SIZE);
  if(cur_task->cr3 != next_task->cr3){
    pg_switch_page_dir(next_task->cr3);
    pg_invalidate_cache();
  }
  
  next_task->state = TS_RUN;
  
  mt_switch_task(next_task);
}

/* Just for debugging */
void mt_print_tasks() {
  task_t *t = tasks;
  printk("\n PID | PPID \t| name");
  printk("\n --- + -------- + ----");
  while (t) {
    printk("\n %d \t | %d \t\t| %s\t", t->pid, t->ppid, t->name);
    t = t->next;
  }
}

#include <drivers/serial.h>
void rush(void *args) {
  while (1) {
    asm volatile("int $0x80");
  }
}

void gpf(void *args) {
  (void)args;

  __asm__ __volatile__("cli");

  mt_task_self_terminate();
  while (1)
    __asm__ __volatile__("pause");
}

void ker_idle(void *args) {
  (void)args;
  while (1) {
    __asm__ __volatile__("hlt");
  }
}

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

extern void irq_common_handler_pop_and_iret(void);
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

  /* Setting up the kernel stack */
  task->esp0 = (uint32_t) kmalloc(task->stack_size);
  uint32_t *stack = (uint32_t *)(task->esp0 + task->stack_size);
  
  /* Arguments and a 0x0 return address */
  /* so do not return, use syscall exit instead */
  /* TODO: exit syscall doesn't exist yet */
  *(--stack) = (uint32_t) args;
  *(--stack) = 0x0;

  /* eflags, cs, eip, that are pushed by the cpu when irq occurs */
  /* there is no ss, esp because there is no priviliage change (ring 0 to ring 0) */
  *(--stack) = 0x200;
  *(--stack) = 0x8;
  *(--stack) = (uint32_t) entry;

  /* Error code and interrupt number */
  /* TODO: Maybe I eliminate error code for IRQs, it makes no sense */
  /*    (Currently I am pushing $0x0 here in the IRQ handlers) */
  stack -= 2;

  /* General purpose registers */
  stack -= 7;

  /* ds es fs gs */
  *(--stack) = 0x10;
  *(--stack) = 0x10;
  *(--stack) = 0x10;
  *(--stack) = 0x10;
  
  *(--stack) = (uint32_t) &irq_common_handler_pop_and_iret;
  
  *(--stack) = 0; /* ebp */
  *(--stack) = 0; /* eax */
  *(--stack) = 0; /* ebx */
  *(--stack) = 0; /* ecx */
  *(--stack) = 0; /* edx */
  *(--stack) = 0; /* esi */
  *(--stack) = 0; /* edi */
  *(--stack) = 0x200; /* eflags */


  task->next = NULL;

  task->esp = (uint32_t) stack;
  task->allocated_stack = (uint32_t) task->esp0;
  task->cr3 = pg_virt_to_phys(pg_get_ker_dir(), (uint32_t) pg_get_ker_dir());

  task->time_slice = DEFAULT_TIME_SLICE;
  task->time_remaining = DEFAULT_TIME_SLICE;
  log_f(INFO, "ktask_create", "Created kernel task (PID %d) : %s", task->pid, name);
  
  return task;
}

#include <exec/elf.h>
#include <fs/vfs.h>

extern void go_usermode(void);
void utask_exec(void *args){
  if(!elf_load((vfs_node_t *) args))
    asm volatile("1: pause; jmp 1b"); /* Until I implement exit() properly */
    
  go_usermode();
}

task_t *utask_create(char *name, uint32_t ppid, char *path, void *args){
  (void) args;
  vfs_node_t *node = vfs_node_from_path(vfs_get_root(), path);
  
  if (!node || node->file->permissions & VFS_DIR)
    return NULL;

  task_t *t = ktask_create(name, ppid, &utask_exec, node);
  t->cr3 = pg_make_user_page_dir();
  return t;
}


void task_destroy(task_t *task){
  kfree((void*)(task->allocated_stack));
  kfree(task);
}

#include <syscalls/syscall.h>
#include <drivers/serial.h>
#include <util/logger.h>
#include <multitasking/scheduler.h>
#include <fs/file_ops.h>
#include <vesa/vesa.h>
#include <mem/paging.h>

#include <string.h>

syscall_t syscall_list[] = {
  &sys_hello,
  &sys_exit,
  NULL,
  &sys_read,
  &sys_write,
  &sys_open,
  &sys_close,
  &sys_getpid,
  &sys_getppid,
  &sys_run,
  &sys_wait,
  NULL,
  &sys_getcwd,
  &sys_setcwd,
  &sys_getfb,
};

void syscall_handler(registers_t *r){
  
  syscall_params_t params = { 
    .eax = r->eax,
    .ebx = r->ebx,
    .ecx = r->ecx,
    .edx = r->edx,
    .esi = r->esi,
    .edi = r->edi,
    .ebp = r->ebp 
  };

  (syscall_list[r->eax])(&params);
  
  r->eax = params.eax;
  r->ebx = params.ebx;
  r->ecx = params.ecx;
  r->edx = params.edx;
  r->esi = params.esi;
  r->edi = params.edi;
  r->ebp = params.ebp;
}

void sys_hello(syscall_params_t *params){
  (void) params;
  //serial_write(SERIAL_COM1, params->ebx);
}

void sys_exit(syscall_params_t *params){
  (void) params;

  task_t *t = mt_get_current_task(); 

  //log_info(NICE_YELLOW_0, "SYSCALL", "PID %d called exit()", t->pid);
  mt_task_terminate(t);

  mt_schedule();

  asm volatile("1: hlt; jmp 1b");
}

void sys_read(syscall_params_t *params){
  
  int fd = (int) params->ebx;
  char *buffer = (char *) params->ecx;
  size_t size = (size_t) params->edx;
  //log_info(NICE_YELLOW_0, "SYSCALL", "read(%d, %d, %d)", fd, buffer, size);
  
  params->eax = read(fd, buffer, size);
}
void sys_write(syscall_params_t *params){

  int fd = (int) params->ebx;
  char *buffer = (char *) params->ecx;
  size_t size = (size_t) params->edx;
  //log_info(NICE_YELLOW_0, "SYSCALL", "write(%d, %d, %d)", fd, buffer, size);
  
  write(fd, buffer, size);
}
void sys_open(syscall_params_t *params){
  char *path = (char *) params->ebx;
  uint32_t flags = (uint32_t) params->ecx;
  uint32_t mode = (uint32_t) params->edx;
  //log_info(NICE_YELLOW_0, "SYSCALL", "open(%s, %d, %d)", path, flags, mode);
  
  params->eax = open(path, flags, mode);
}
void sys_close(syscall_params_t *params){
  (void) params;
  int fd = (int) params->ebx;
  
  params->eax = close(fd);
}

void sys_getpid(syscall_params_t *params){
  (void) params;
  params->eax = mt_get_current_task()->pid;
}
void sys_getppid(syscall_params_t *params){
  (void) params;
  params->eax = mt_get_current_task()->ppid;
}

void sys_run(syscall_params_t *params){
  char *path = (char *) params->ebx;
  //int   argc = (char *) params->ecx;
  //char *argv = (char *) params->edx;
  
  vfs_node_t *cwd = mt_get_current_task()->cwd_node;
  vfs_node_t *node = vfs_node_from_path(cwd, path);
  //log_info(NICE_YELLOW_0, "SYSCALL", "run(%s)", vfs_abs_path_to(node));
  
  uint32_t ppid = mt_get_current_task()->pid;
  uint32_t pid = mt_spawn_utask(path, ppid, vfs_abs_path_to(node), NULL);
  params->eax = pid;
}

void sys_wait(syscall_params_t *params){
  uint32_t pid = params->ebx;

  mt_set_fg_task(mt_get_task_by_pid(pid));

  /* TODO : Make a better scheduler, and just block the waiting task */
  while(mt_get_task_by_pid(pid))
    asm volatile("hlt");
  
  mt_set_fg_task(mt_get_current_task());

  /* TODO : Return the status code */
  params->eax = 0;
}

void sys_getcwd(syscall_params_t *params){
  char *buffer       = (char *) params->ebx;
  size_t buffer_size = (size_t) params->ecx;

  char *cwd    = vfs_abs_path_to(mt_get_current_task()->cwd_node);
  size_t len   = strlen(cwd);
  
  size_t to_copy = buffer_size < len ? buffer_size : len;
  memcpy(buffer, cwd, to_copy);
}

void sys_setcwd(syscall_params_t *params){
  char *path = (char *) params->ebx;
  //log_info(NICE_YELLOW_0, "SYSCALL", "setcwd(%s)", path);
  
  vfs_node_t *cwd = mt_get_current_task()->cwd_node;
  vfs_node_t *node = vfs_node_from_path(cwd, path);
  
  if(!node) return;

  mt_get_current_task()->cwd_node = node;
}

void sys_getfb(syscall_params_t *params){
  (void) params;
  /* Make the framebuffer accessible from usermode */
  uint32_t pg_dir  = pg_get_current_dir();
  uint32_t fb_addr = (uint32_t) vesa_get_framebuffer();
  uint32_t bfb_addr = (uint32_t) vesa_get_back_buffer();
  /*
    - 1 page is 4 KiB = 4096 Bytes
    - The framebuffer has width*height pixels
    - Each pixel has bpp bytes
    - So in total we have b = width*height*bpp bytes in the fb
    - Therefore we need to map (b / 4096) pages
  */
  uint32_t w   = vesa_get_framebuffer_width();
  uint32_t h   = vesa_get_framebuffer_height();
  uint32_t bpp = vesa_get_framebuffer_bpp();
  
  uint32_t pages_count = w * h * bpp / PG_SIZE;
  
  uint32_t offset = 0;
  while(pages_count >= 1024){
    pages_count -= 1024;
    offset += 4096;
    pg_map_pages(pg_dir, fb_addr+offset, fb_addr+offset, 1024, PG_PRESENT | PG_USER | PG_RW);
    //pg_map_pages(pg_dir, bfb_addr+offset, bfb_addr+offset, 1024, PG_PRESENT | PG_USER | PG_RW);
  
  }
  /* I think that this loop thing should actually be inside pg_map_pages */
  /* TODO : do it */
//log_info(INFO, "fb", "%d", bfb_addr);

  if(pages_count){
    pg_map_pages(pg_dir, fb_addr+offset, fb_addr+offset, pages_count, PG_PRESENT | PG_USER | PG_RW);
    //pg_map_pages(pg_dir, bfb_addr+offset, bfb_addr+offset, pages_count, PG_PRESENT | PG_USER | PG_RW);
  
  }
  /* I hope I got that right */
  /* Hey future me, I am sorry if you came here because of a bug here */
  //log_info(INFO, "fb", "%d", bfb_addr);
  *((uint32_t *)params->ebx) = (uint32_t) vesa_get_back_buffer();
  params->eax = (uint32_t) vesa_get_framebuffer();
}

#include <ds/list.h>
#include <fs/file_descriptor.h>
#include <mem/heap.h>
#include <multitasking/schedlock.h>
#include <multitasking/scheduler.h>

list_t *gft;

uint8_t gft_init() {
  gft = make_list(1024);

  /*
      Note :  Let's start from 1 so that gft_entry_exists
              can safely return 0 when entry doesn't exist
  */
  gft->used = 1;

  return gft == NULL;
}

static uint32_t gft_entry_exists(vfs_node_t *node) {
  return 0;
  for (uint32_t i = 1; i <= gft->used; i++) {
    gft_entry_t *e = (gft_entry_t *)list_get(gft, i);
    if (e && e->node == node)
      return i;
  }
  return 0;
}

static void gft_inc_ref_count(uint32_t index) {
  ((gft_entry_t *)list_get(gft, index))->ref_count++;
}

static void gft_dec_ref_count(uint32_t index) {
  ((gft_entry_t *)list_get(gft, index))->ref_count--;
}

static uint32_t gft_get_ref_count(uint32_t index) {
  return ((gft_entry_t *)list_get(gft, index))->ref_count;
}

uint32_t gft_add(vfs_node_t *node) {
  uint32_t ef;
  ef = get_eflags_and_cli();

  uint32_t i;
  i = gft_entry_exists(node);
  if (i) {
    gft_inc_ref_count(i);
    return i;
  }

  gft_entry_t *e;
  e = (gft_entry_t *)kmalloc(sizeof(gft_entry_t));

  e->node = node;
  e->locked_by = 0;
  e->ref_count = 1;

  i = list_push(gft, e);

  set_eflags(ef);
  return i;
}

uint8_t gft_delete(uint32_t index) {
  uint32_t ef;
  ef = get_eflags_and_cli();

  list_delete(gft, index);

  set_eflags(ef);
  return 0;
}

uint8_t gft_lock(uint32_t index) {
  uint32_t ef;
  ef = get_eflags_and_cli();

  gft_entry_t *e;
  e = list_get(gft, index);

  e->locked_by = mt_get_current_task()->pid;

  set_eflags(ef);
  return 0;
}

uint8_t gft_unlock(uint32_t index) {
  uint32_t ef;
  ef = get_eflags_and_cli();

  gft_entry_t *e;
  e = list_get(gft, index);

  if (e->locked_by != mt_get_current_task()->pid) {
    set_eflags(ef);
    return 1;
  }

  e->locked_by = 0;

  set_eflags(ef);
  return 0;
}

file_descriptor_t *fd_open(vfs_node_t *node, uint32_t flags) {
  uint32_t ef;
  ef = get_eflags_and_cli();

  uint8_t read = (flags & O_RDWR) || (flags & O_RDONLY);
  uint8_t write = (flags & O_RDWR) || (flags & O_WRONLY);

  uint32_t index = gft_add(node);
  file_descriptor_t *fd;
  fd = (file_descriptor_t *)kmalloc(sizeof(file_descriptor_t));

  fd->gft_index = index;
  fd->read = read;
  fd->write = write;
  fd->offset = 0;

  set_eflags(ef);
  return fd;
}

uint8_t fd_close(file_descriptor_t *fd) {
  uint32_t ef;
  ef = get_eflags_and_cli();

  gft_dec_ref_count(fd->gft_index);

  if (gft_get_ref_count(fd->gft_index) == 0)
    gft_delete(fd->gft_index);
  kfree(fd);

  set_eflags(ef);
  return 0;
}

uint8_t fd_seek(file_descriptor_t *fd, uint32_t offset) {
  uint32_t ef;
  ef = get_eflags_and_cli();

  fd->offset = offset;

  set_eflags(ef);
  return 0;
}

uint8_t fd_lock(file_descriptor_t *fd) {
  uint32_t ef;
  ef = get_eflags_and_cli();

  gft_lock(fd->gft_index);

  set_eflags(ef);
  return 0;
}

uint8_t fd_unlock(file_descriptor_t *fd) {
  uint32_t ef;
  ef = get_eflags_and_cli();

  gft_unlock(fd->gft_index);

  set_eflags(ef);
  return 0;
}

uint32_t fd_locked_by(file_descriptor_t *fd) {
  uint32_t ef;
  ef = get_eflags_and_cli();

  gft_entry_t *e;
  e = list_get(gft, fd->gft_index);

  set_eflags(ef);
  return e->locked_by;
}

inline vfs_node_t *fd_to_node(file_descriptor_t *fd) {
  gft_entry_t *e = (gft_entry_t *)list_get(gft, fd->gft_index);
  return e->node;
}

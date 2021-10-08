#include <drivers/console.h>
#include <fs/devfs/devfs.h>
#include <term/term.h>

uint32_t console_write(vfs_file_t *file, uint32_t offset, uint32_t size,
                   char *buffer) {
                   
  for(size_t i = 0; i < size; i++){
    term_putchar(buffer[i]);
  }
  
  return size;
}

void console_init() {
  devfs_node_t *console_dev;
  console_dev = (devfs_node_t *) kmalloc(sizeof(devfs_node_t));
  console_dev->name = "con";
  console_dev->type = VFS_CDEV;
  
  /* TODO : maybe I'll make read() read from keyboard */
  console_dev->read = 0;
  console_dev->write = &console_write;
    
  devfs_register_dev(console_dev);
}

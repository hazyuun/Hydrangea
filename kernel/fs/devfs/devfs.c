#include <fs/devfs/devfs.h>

vfs_node_t *devfs_root = 0;

uint8_t devfs_init(char *path) {
  if (devfs_root)
    return 1;
  devfs_root = vfs_node_from_path(vfs_get_root(), path);
  return 0;
}

uint8_t devfs_register_dev(devfs_node_t *dev) {
  if (!devfs_root)
    return 1;

  if (dev->type != VFS_CDEV && dev->type != VFS_BDEV)
    return 2;

  vfs_node_t *node = vfs_add_child(devfs_root, dev->name, dev->type);
  node->file->open = dev->open;
  node->file->read = dev->read;
  node->file->write = dev->write;
  node->file->close = dev->close;
  node->file->inode = dev->device_specific;
  return 0;
}

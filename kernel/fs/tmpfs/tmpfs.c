#include <fs/tmpfs/tmpfs.h>
#include <util/logger.h>
#include <string.h>

vfs_node_t *tmpfs_root = 0;
/* 
    TODO: Maybe allow tmpfs to be used in more than one directory ?
    I mean, initrd is technically just a tmpfs with extra steps
    
    Edit: I should've started with this tbh
 */

uint8_t tmpfs_init(char *path) {
  if(tmpfs_root) return 1;
  tmpfs_root = vfs_node_from_path(vfs_get_root(), path);
  return 0;
}

/* Note: path should be relative to tmpfs_root */
uint32_t tmpfs_create(char *path, uint8_t type){
  if(!tmpfs_root) return 255;
  if(type != VFS_FILE && type != VFS_DIR) return 254;
  /*
    Okay, this is gonna be very annoying actually, 
    let's see what could go wrong here, path may actually be
    pointing to something outside tmp, let's see
    
    path                ->   is valid ?
    
    file.txt            ->   valid 
    /file.txt           ->   invalid
    ../file.txt         ->   invalid
    test/file.txt       ->   valid only if "test" directory exists
    ./file.txt          ->   valid
    test/../../file.txt ->   invalid (if test exists then it is equivalent to ../file.txt)
    
    To check path validity I may do this :
    
    1/ Take path, split it in the last '/', and take the first part
        
        test/../../lol/file.txt  ---> test/../../lol
        
    2/ Try to get its vfs_node_t
        |
        * Does it exist ? 
        + No  => Invalid path
        + Yes => * Is it inside tmpfs_root
        :        + Yes => valid
        :        + No  => invalid
        
    Edit :
      Well, the only way to actually leave tmpfs_root is with ".."
      (paths starting with '/' are rejected straight away)
      meaning that I can just count the dots and.. 
      you know what, f*ck this shit, I am gonna just ban ".."
      inside the path here, problem solved !

    Edit : 
      Why do I even care, I am not trying to make a secure kernel at all.
      I'm gonna ban ".." here anyway.
  */
  
  if(path[0] == '/')
    return 1;
  
  if(strstr(path, "..") != NULL)
    return 2;
  
  uint32_t N = strlen(path);
  if(path[N - 1] == '/')
    return 3;
  
  uint32_t last_slash_index = 0;
  for(int32_t i = N - 1; i >= 0; i--){
    if(path[i] == '/'){
      last_slash_index = i;
      break;
    }
  }
  
  if(last_slash_index == 0){
    vfs_node_t *node = vfs_add_child(tmpfs_root, path, type);
    node->file->inode = 0;
    node->file->read = tmpfs_vfs_read;
    node->file->write = tmpfs_vfs_write;
    return 0;
  }
  
  char parent_path[last_slash_index + 1];
  char filename[N - last_slash_index + 2];
  
  memcpy(parent_path, path, last_slash_index);
  memcpy(filename, path + last_slash_index + 1, N - last_slash_index + 1);
  parent_path[last_slash_index] = 0;
  filename[N - last_slash_index + 1] = 0;
  
  
  vfs_node_t *parent = vfs_node_from_path(tmpfs_root, parent_path);
  if(!parent){
    log_f(INFO, "tmpfs", "Failed to create %s, parent directory (%s) doesn't exist ", filename, parent_path);
    return 4;
  }
  vfs_node_t *node = vfs_add_child(parent, filename, type);
  node->file->inode = 0;
  node->file->read = tmpfs_vfs_read;
  node->file->write = tmpfs_vfs_write;
  
  //log_f(INFO, "tmpfs", "parent : %s | filename : %s", parent_path, filename);
  
  return 0;
}


uint32_t tmpfs_vfs_read(vfs_file_t *node, uint32_t offset, uint32_t size,
                       char *buffer) {
  if (offset >= node->size)
    return 0;
  if (offset + size > node->size)
    size = node->size - offset;
  memcpy(buffer, (uint8_t *)node->inode + offset, size);
  return size;
}

uint32_t tmpfs_vfs_write(vfs_file_t *node, uint32_t offset, uint32_t size,
                        char *buffer) {
  if(!node->inode)
    node->inode = kmalloc(size);
  if(node->size < size)
    node->inode = krealloc(node->inode, size);
  node->size += size;
  
  memcpy((uint8_t *)node->inode + offset, buffer, size);
  return size;
}

#if 0
uint8_t tmpfs_vfs_open(vfs_file_t *node, uint8_t read, uint8_t write) {

}

uint8_t tmpfs_vfs_close(vfs_file_t *node) {

}

vfs_dirent_t *tmpfs_vfs_readdir(vfs_file_t *node, uint32_t index) {

}

vfs_file_t *tmpfs_vfs_finddir(vfs_file_t *node, char *name) {

}
#endif

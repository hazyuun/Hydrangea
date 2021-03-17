#if !defined(__i386__) || defined(__linux__)
#error "ERR : Please use a ix86-elf cross-compiler"
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <boot/multiboot.h>
#include <kernel.h>

#include <cpu/gdt.h>
#include <cpu/idt.h>
#include <cpu/pit.h>

#include <mem/heap.h>
#include <mem/paging.h>
#include <mem/pmm.h>

#include <drivers/kbd.h>
#include <drivers/pci.h>
#include <drivers/rtc.h>
#include <drivers/serial.h>
#include <drivers/ata.h>

#include <misc/mbr.h>

#include <fs/initrd/initrd.h>

#include <term/term.h>
#include <stdio.h>
#include <string.h>
#include <util/logger.h>

void kmain(uint32_t mb_magic, multiboot_info_t *mbi) {
  if(term_init(VESA_TERM, mbi))
    hang();

  log_info(INFO, "INFO", "Kernel loaded !");
  
  if (mb_magic != MULTIBOOT_BOOTLOADER_MAGIC)
    panic("Invalid multiboot magic number !");

  if (!(mbi->flags & (1 << 6)))
    panic("No multiboot memory map !");

  pmm_init(mbi);
  log_info(INFO, "INFO", "Available memory : %d KiB\n", pmm_available_memory());
  
  gdt_init();
  idt_init();
  serial_init(SERIAL_COM1);
  
  pg_init(mbi);
  pit_init(100);
  vfs_dummy();

  if (mbi->flags & (1 << 3))
    initrd_init(mbi);

  log_result(!PCI_detect(), "Detecting and initializing PCI devices"); 

  printk("\n\nWelcome to ");
  term_use_color(NICE_MAGENTA);
  printk("YuunOS !\n");
  term_use_color(NICE_WHITE);

  kbd_switch_layout("en");
  /* This is a quick and dirty and temporary cli */
  /* just for the sake of testing ! */
  /* Edit : it is getting messy lol */
  vfs_node_t *cwd = vfs_get_root();
  char cmd[100];
  while (1) {
    term_use_color(NICE_CYAN_0);
    printk("\nKernel ");
    term_use_color(NICE_CYAN);
    printk("%s", vfs_abs_path_to(cwd));
    term_use_color(NICE_WHITE);
    printk("> ");

    scank("%s", cmd);
    if (!strcmp("info", cmd)) {
      term_print("YuunOS "KERNEL_VERSION "\n");
    } 
    
    else if (!strcmp("clear", cmd)) {
      term_clear();
    } 
    
    else if (!strcmp("reboot", cmd)) {
      // TODO: move this shit elsewhere
      uint8_t TW = 0x02;
      while (TW & 0x02)
        TW = io_inb(0x64);
      io_outb(0x64, 0xFE);
      while (1)
        ;
    } 
    
    else if (!strcmp("sleep", cmd)) { /* Will sleep 10 secs */
      pit_sleep(10 * 100);
    }
    
    else if (!strcmp("datetime", cmd)) {
      rtc_print_now();
    }
    
    else if (!strcmp("lspci", cmd)) {
      PCI_list();
    }
    
    else if (!strcmp(cmd, "ls")) {
      vfs_node_t *node = cwd->childs;
      while (node) {
        if (vfs_is_dir(node))
          term_use_color(NICE_YELLOW);
        else if (vfs_is_mtpt(node))
          term_use_color(NICE_RED);
        else
          term_use_color(NICE_WHITE);
        printk("%s \t", node->name);
        node = node->next;
      }
      printk("\n");
      term_use_color(NICE_WHITE);
    }
    
    else if (!strcmp(cmd, "ll")) {
      vfs_node_t *node = cwd->childs;
      while (node) {
        
        char drwxrwxrwx[10];
        vfs_drwxrwxrwx(drwxrwxrwx, node->file->permissions);
        printk("\n %s user group %dB \t", drwxrwxrwx, node->file->size);

        if (vfs_is_dir(node))
          term_use_color(NICE_YELLOW);
        else if (vfs_is_mtpt(node))
          term_use_color(NICE_RED);
        else
          term_use_color(NICE_WHITE);

        printk("%s", node->name);
        term_use_color(NICE_WHITE);
        
        node = node->next;
      }
      printk("\n");
      term_use_color(NICE_WHITE);
    }

    else if (!strcmp(cmd, "ki")) {
      vfs_show_tree(cwd, 0);
    }
    
    else if (!strcmp(cmd, "lsdrv")) {
      ATA_print_infos();
    }
    
    
    else if (strcmp(cmd, "")) {
      char *token = strtok(cmd, " ");
      
      if (!strcmp(token, "cd")) {
        token = strtok(NULL, " ");
        vfs_node_t *n;

        if ((n = vfs_abspath_to_node(cwd, token))) {
          if (vfs_is_dir(n) || vfs_is_mtpt(n))
            cwd = n;
          else
            printk("Not a directory");
        } else {
          printk("Not found");
        }
      }
      
      else if (!strcmp("kbd", cmd)) {
        token = strtok(NULL, " ");
        if (!strcmp(token, "fr") || !strcmp(token, "en")) {
          kbd_switch_layout(token);

          printk("Keyboard layout changed to : %s%s", token,
                 !strcmp(token, "fr") ? "\nEnjoy your baguette !\n" : "\n");
        } else
          printk("Invalid keyboard layout\n");

      }
      
      else if (!strcmp("cat", cmd)) {
        token = strtok(NULL, " ");

        vfs_node_t *node = vfs_get_child_by_name(cwd, token);
        if (!node)
          printk("Not found");
        else {
          printk("\nReading : %d Bytes \n---\n", node->file->size);
          char *contents = kmalloc(node->file->size + 1);
          memset(contents, 0, node->file->size + 1);
          if (vfs_read(node->file, 0, node->file->size, contents))
            printk("%s", contents);
          kfree(contents);
        }

      }

      else if (!strcmp("mbr", cmd)) {
        uint8_t ms = atoi(strtok(NULL, " "));
        uint8_t ps = atoi(strtok(NULL, " "));
        if((ms != 0 && ms != 1)
        || (ps != 0 && ps != 1))
          printk("Invalid drive\n");
        else{
          ATA_drive_t *drv = ATA_get_drive(ms, ps);
          if(!drv) printk("Drive not found\n");
          else {
            mbr_t mbr;
            mbr_parse(drv, &mbr);
            mbr_print(&mbr);
          }
        }
      }

      else if (!strcmp("mount", cmd)) {
        uint8_t ms = atoi(strtok(NULL, " "));
        uint8_t ps = atoi(strtok(NULL, " "));
        uint8_t part = atoi(strtok(NULL, " "));
        char *path = strtok(NULL, " ");
        if((ms != 0 && ms != 1)
        || (ps != 0 && ps != 1))
          printk("Invalid drive\n");
        else if(part > 3)
          printk("Invalid partition number\n");
        else{
          ATA_drive_t *drv = ATA_get_drive(ms, ps);
          if(!drv) printk("Drive not found\n");
          else {
            uint8_t err = vfs_mount_partition(drv, part, path, cwd);
            if(err == 1){
              printk("Unknown filesystem");
            } else if(err == 2){
              printk("%s not found", path);
            } else if(err == 3){
              printk("%s not empty", path);
            } else if(err == 255){
              printk("Already mounted at %s", vfs_abs_path_to((vfs_node_t*) drv->mtpts[part]));
            }
          }
        }
      }

      else if (!strcmp("umount", cmd)) {
        uint8_t ms = atoi(strtok(NULL, " "));
        uint8_t ps = atoi(strtok(NULL, " "));
        uint8_t part = atoi(strtok(NULL, " "));

        if((ms != 0 && ms != 1)
        || (ps != 0 && ps != 1))
          printk("Invalid drive\n");
        else if(part > 3)
          printk("Invalid partition number\n");
        else{
          ATA_drive_t *drv = ATA_get_drive(ms, ps);
          if(!drv) printk("Drive not found\n");
          else {
            uint8_t err = vfs_umount_partition(drv, part);
            if(err == 1){
              printk("Not mounted");
            }
          }
        }
      }

      

      else
        printk("Unknown command\n");
    }
  }
  
  hang();
}

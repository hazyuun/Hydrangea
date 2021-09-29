#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <boot/multiboot.h>
#include <kernel.h>

#include <cpu/gdt.h>
#include <cpu/interrupts.h>

#include <mem/heap.h>
#include <mem/paging.h>
#include <mem/pmm.h>

#include <drivers/pit.h>
#include <drivers/kbd.h>
#include <drivers/mouse.h>
#include <drivers/ps2.h>
#include <drivers/kbd.h>
#include <drivers/mouse.h>


#include <drivers/pci.h>
#include <drivers/rtc.h>
#include <drivers/serial.h>
#include <drivers/ata.h>

#include <misc/mbr.h>

#include <fs/initrd/initrd.h>

#include <multitasking/scheduler.h>

#include <term/term.h>
#include <stdio.h>
#include <string.h>
#include <util/logger.h>
#include <exec/elf.h>

static void check_multiboot_info(uint32_t mb_magic, multiboot_info_t *mbi){
  if (mb_magic != MULTIBOOT_BOOTLOADER_MAGIC)
    panic("Invalid multiboot magic number !");

  if (!(mbi->flags & (1 << 6)))
    panic("No multiboot memory map !");
}

__attribute__((noreturn)) void quick_and_dirty_kernel_cli();
extern uint32_t vesa_width, vesa_height, vesa_pitch, vesa_bpp;

#include <util/logger.h>
__attribute__((noreturn)) void kmain(uint32_t mb_magic, multiboot_info_t *mbi) {
  check_multiboot_info(mb_magic, mbi);
  pmm_init(mbi);
  gdt_init();
  interrupts_init();
  serial_init(SERIAL_COM1);
  pg_init(mbi);

  if(term_init(VESA_TERM, mbi))
    hang();

  log_info(INFO, "INFO", "Kernel loaded !");
  log_f(INFO, "", "%dx%d %d %d", vesa_width, vesa_height, vesa_pitch, vesa_bpp);
  
  log_info(INFO, "INFO", "Available memory : %d KiB\n", pmm_available_memory());
  
  pit_init(1000);
  
  // ps2_init();
  kbd_init(1);
    
  vfs_dummy();
  initrd_init(mbi);

  log_result(!PCI_detect(), "Detecting and initializing PCI devices"); 

  printk("\n\nWelcome to ");
  term_use_color(NICE_MAGENTA);
  printk("YuunOS !\n");
  term_use_color(NICE_WHITE);

  mt_init();
  mt_spawn_ktask("cli", 0, &quick_and_dirty_kernel_cli, NULL);

  hang();
}

/* This is a quick and dirty and temporary cli */
/* just for the sake of testing ! */
/* Edit : it is getting messy lol */  
/* Edit : I can't wait to replace this with an actual shell in userland */
/* Edit : Usermode works ! I am getting closer and closer to a userland shell */
/* Edit : I just wrote a (shitty) ELF loader, Now I just need enough syscalls */
/*        for a userspace shell ! So far so good */
__attribute__((noreturn)) void quick_and_dirty_kernel_cli(){
  vfs_node_t *cwd = vfs_get_root();
  //mt_spawn_utask("hello", 0, "/initrd/0/initrd/hello.elf", 0);
  //hang();
  char cmd[100] = "\0";
  
  while (1) {
    term_use_color(NICE_CYAN_0);
    printk("\nKernel ");
    term_use_color(NICE_CYAN);
    printk("%s", vfs_abs_path_to(cwd));
    term_use_color(NICE_WHITE);
    printk("> ");

    scank("%s", cmd);
    if (!strcmp("info", cmd)) {
      term_print("YuunOS kernel "KERNEL_VERSION "\n");
    } 
    
    else if (!strcmp("clear", cmd)) {
      term_clear();
    }

    else if (!strcmp("help", cmd)) {
      printk("Too lazy to write help, sorry\n");
      printk("This shitty \"shell\" will be replaced anyway\n");
    } 
    
    else if (!strcmp("reboot", cmd)) {
      // TODO: move this shit elsewhere
      /* EDIT : Yo ! I am you from future, BRUH WTF THIS IS CURSED ! */
      uint8_t TW = 0x02;
      while (TW & 0x02)
        TW = io_inb(0x64);
      io_outb(0x64, 0xFE);
      while (1)
        ;
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

        if ((n = vfs_node_from_path(cwd, token))) {
          if (vfs_is_dir(n) || vfs_is_mtpt(n))
            cwd = n;
          else
            printk("Not a directory");
        } else {
          printk("Not found");
        }
      }
      else if (!strcmp("sleep", cmd)) {
        uint8_t s = atoi(strtok(NULL, " "));
        pit_sleep(s * 100);
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

      else if (!strcmp("term", cmd)) {
        uint32_t pid = atoi(strtok(NULL, " "));
        mt_task_terminate_pid(pid);
      }

      else if (!strcmp("pid", cmd)) {
        uint32_t pid = atoi(strtok(NULL, " "));
        printk("PID %d : %s \n", pid, mt_get_task_by_pid(pid)->name);
      }

      else if (!strcmp("tasks", cmd)) {
        
        mt_print_tasks();
      }
      else if (!strcmp("ttasks", cmd)) {
        mt_print_terminated_tasks();
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
          printk("\nInvalid drive\n");
        else if(part > 3)
          printk("\nInvalid partition number\n");
        else{
          ATA_drive_t *drv = ATA_get_drive(ms, ps);
          if(!drv) printk("\nDrive not found\n");
          else {
            uint8_t err = vfs_mount_partition(drv, part, path, cwd);
            if(err == 1){
              printk("\nUnknown filesystem");
            } else if(err == 2){
              printk("\n%s not found", path);
            } else if(err == 3){
              printk("\n%s not empty", path);
            } else if(err == 255){
              printk("\nAlready mounted at %s", vfs_abs_path_to((vfs_node_t*) drv->mtpts[part]));
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

      else {
        //printk("Unknown command\n");
        if(!mt_spawn_utask(cmd, 0, cmd, 0))
          printk("Unknown command\n");
        
      }
    }
  }
}

__attribute__((noreturn)) inline void hang(void){
  do{
    __asm__ __volatile__("hlt\n\t");
  }while(1);
}

void panic(char *err_msg){
  __asm__ __volatile__("cli");
  mt_panic();
  term_use_color(NICE_RED);
  printk("\n KERNEL PANIC : %s", err_msg);
  hang();
}

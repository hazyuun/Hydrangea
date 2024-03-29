#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include <boot/multiboot.h>
#include <kernel.h>

#include <cpu/gdt.h>
#include <cpu/interrupts.h>

#include <mem/heap.h>
#include <mem/paging.h>
#include <mem/pmm.h>

#include <drivers/ata.h>
#include <drivers/console.h>
#include <drivers/kbd.h>
#include <drivers/mouse.h>
#include <drivers/pci.h>
#include <drivers/pit.h>
#include <drivers/ps2.h>
#include <drivers/rtc.h>
#include <drivers/serial.h>

#include <misc/mbr.h>

#include <fs/devfs/devfs.h>
#include <fs/file_descriptor.h>
#include <fs/file_ops.h>
#include <fs/initrd/initrd.h>
#include <fs/pipe.h>
#include <fs/tmpfs/tmpfs.h>
#include <fs/vfs.h>

#include <exec/elf.h>
#include <multitasking/scheduler.h>

#include <syscalls/syscall.h>

#include <term/term.h>
#include <util/logger.h>

static void check_multiboot_info(uint32_t mb_magic, multiboot_info_t *mbi) {
  if (mb_magic != MULTIBOOT_BOOTLOADER_MAGIC)
    panic("Invalid multiboot magic number !");

  if (!(mbi->flags & (1 << 6)))
    panic("No multiboot memory map !");
}

__attribute__((noreturn)) void kshell();

__attribute__((noreturn)) void kmain(uint32_t mb_magic, multiboot_info_t *mbi) {
  check_multiboot_info(mb_magic, mbi);
  gdt_init();
  pmm_init(mbi);
  interrupts_init();
  pg_init(mbi);

  if (term_init(VESA_TERM, mbi))
    hang();

  vfs_dummy();
  devfs_init("/dev");
  tmpfs_init("/tmp");

  log_info(INFO, "INFO", "Kernel loaded !");
  log_info(INFO, "INFO", "Available memory : %d KiB\n", pmm_available_memory());

  serial_init(SERIAL_COM1);
  serial_init(SERIAL_COM2);
  serial_init(SERIAL_COM3);
  serial_init(SERIAL_COM4);
  console_init();

  pit_init(1000);

  // ps2_init();
  kbd_init(1);

  initrd_init(mbi);

  log_result(!PCI_detect(), "Detecting and initializing PCI devices");
  mt_init();

  printk("\n\nWelcome to ");
  term_use_color(NICE_CYAN_0);
  printk("Hydrangea !\n");
  term_use_color(NICE_WHITE);

  mt_spawn_ktask("cli", 0, &kshell, NULL);

  hang();
}

/* This is a temporary cli running in kernel mode just for the sake of testing !
 */
__attribute__((noreturn)) void kshell() {
  vfs_node_t *cwd = vfs_get_root();
  cwd = vfs_node_from_path(cwd, "/");
  char x[] = "/";
  syscall_params_t p = {.ebx = (uint32_t)x};
  sys_setcwd(&p);
  
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
      term_print(KERNEL_NAME " " KERNEL_VERSION "\n");
    }

    else if (!strcmp("clear", cmd)) {
      term_clear();
    }

    else if (!strcmp("help", cmd)) {
      printk("Too lazy to write help, sorry\n");
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
        char *cwd_ = vfs_abs_path_to(cwd);
        syscall_params_t p = {.ebx = (uint32_t)cwd_};
        sys_setcwd(&p);
      } else if (!strcmp("sleep", cmd)) {
        uint8_t s = atoi(strtok(NULL, " "));
        pit_sleep(s * 100);
      } else if (!strcmp("kbd", cmd)) {
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
      /*
      else if (!strcmp("ttasks", cmd)) {
        mt_print_terminated_tasks();
      }
      */
      else if (!strcmp("mbr", cmd)) {
        uint8_t ms = atoi(strtok(NULL, " "));
        uint8_t ps = atoi(strtok(NULL, " "));
        if ((ms != 0 && ms != 1) || (ps != 0 && ps != 1))
          printk("Invalid drive\n");
        else {
          ATA_drive_t *drv = ATA_get_drive(ms, ps);
          if (!drv)
            printk("Drive not found\n");
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
        if ((ms != 0 && ms != 1) || (ps != 0 && ps != 1))
          printk("\nInvalid drive\n");
        else if (part > 3)
          printk("\nInvalid partition number\n");
        else {
          ATA_drive_t *drv = ATA_get_drive(ms, ps);
          if (!drv)
            printk("\nDrive not found\n");
          else {
            uint8_t err = vfs_mount_partition(drv, part, path, cwd);
            if (err == 1) {
              printk("\nUnknown filesystem");
            } else if (err == 2) {
              printk("\n%s not found", path);
            } else if (err == 3) {
              printk("\n%s not empty", path);
            } else if (err == 255) {
              printk("\nAlready mounted at %s",
                     vfs_abs_path_to((vfs_node_t *)drv->mtpts[part]));
            }
          }
        }
      }

      else if (!strcmp("umount", cmd)) {
        uint8_t ms = atoi(strtok(NULL, " "));
        uint8_t ps = atoi(strtok(NULL, " "));
        uint8_t part = atoi(strtok(NULL, " "));

        if ((ms != 0 && ms != 1) || (ps != 0 && ps != 1))
          printk("Invalid drive\n");
        else if (part > 3)
          printk("Invalid partition number\n");
        else {
          ATA_drive_t *drv = ATA_get_drive(ms, ps);
          if (!drv)
            printk("Drive not found\n");
          else {
            uint8_t err = vfs_umount_partition(drv, part);
            if (err == 1) {
              printk("Not mounted");
            }
          }
        }
      }

      else {
        // printk("Unknown command\n");
        vfs_node_t *n = vfs_node_from_path(cwd, cmd);
        if (!n)
          printk("Unknown command\n");
        else {
          char *abs = vfs_abs_path_to(n);
          uint32_t pid =
              mt_spawn_utask(cmd, mt_get_current_task()->pid, abs, 0);

          if (!pid)
            printk("Couldn't spawn new task\n");
          else {
            task_t *t = mt_get_task_by_pid(pid);
            mt_set_fg_task(t);

            /* Ah yes, wait */
            while (mt_get_task_by_pid(pid))
              asm volatile("pause");
            mt_set_fg_task(mt_get_current_task());
          }
        }
      }
    }
  }
}

__attribute__((noreturn)) inline void hang(void) {
  do {
    __asm__ __volatile__("hlt\n\t");
  } while (1);
}

void panic(char *err_msg) {
  __asm__ __volatile__("cli");
  mt_panic();
  term_use_color(NICE_RED);
  printk("\n KERNEL PANIC : %s", err_msg);
  hang();
}

#include <stdarg.h>
#include <stdio.h>
#include <term/term.h>
#include <util/logger.h>

void v_log_info(uint32_t color, const char *who, char *format, va_list args){
  term_use_color(NICE_WHITE);
  printk("\n [ ");
  term_use_color(color);
  printk("%s", who);
  term_use_color(NICE_WHITE);
  printk(" ] ");
  v_printk(format, args);
  term_use_color(NICE_WHITE);
}

void log_info(uint32_t color, const char *who, char *format, ...){
  va_list args;
  va_start(args, format);
  v_log_info(color, who, format, args);
  va_end(args);
}

void log_f(int64_t lvl, const char *who, char *format, ...) {
  va_list args;
  va_start(args, format);
  term_use_color(NICE_WHITE);

  printk("\n [ ");
  term_use_color((uint32_t)lvl);
  switch (lvl) {
  case OKAY:
    printk("OKAY");
    break;
  case WARN:
    printk("WARN");
    break;
  case ERROR:
    printk("ERROR");
    break;
  case INFO:
    printk("INFO");
    break;
  }

  term_use_color(NICE_WHITE);
  printk(" ] %s : ", who);
  v_printk(format, args);
  term_use_color(NICE_WHITE);
  va_end(args);
}

void log_result(uint8_t condition, char *format, ...){
  va_list args;
  va_start(args, format);
  if(condition)
    v_log_info(OKAY, "OKAY", format, args);
  else
    v_log_info(ERROR, "FAIL", format, args);
  va_end(args);
}

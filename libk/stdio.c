#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include <drivers/kbd.h>
#include <tty/tty.h>

void printk(const char *format, ...) {
  va_list ap;

  va_start(ap, format);
  while (*format != '\0') {
    if (*format != '%')
      tty_putchar(*format++);
    else {
      format++;
      switch (*format) {
      case 'c': {
        tty_putchar(va_arg(ap, int));
        break;
      }
      case 's': {
        tty_print(va_arg(ap, char *));
        break;
      }
      case 'd': {
        char str[20];
        itoa(va_arg(ap, int), str, 10);
        tty_print(str);
        break;
      }
      case 'f': {
        /* Hacky float printing */
        /* TODO: Print floats correctly */
        double f = va_arg(ap, double);
        if (f < 0) {
          tty_putchar('-');
          f *= -1;
        }
        int whole = (int)f;
        printk("%d.", whole);

        for (int n = 0; n < 4; n++) {
          f -= whole;
          f *= 10.0;
          whole = (int)f;
          printk("%d", whole);
        }

        break;
      }
      case 'x': {
        char str[20];
        itoa(va_arg(ap, int), str, 16);
        tty_print(str);
        break;
      }
      default: {
        tty_putchar(*(--format));
        tty_putchar(*(++format));
        break;
      }
      }
      format++;
    }
  }
  va_end(ap);
}

void scank(const char *format, ...) {
  va_list ap;

  va_start(ap, format);

  while (*format != '\0') {
    if (*format != '%') {
    } else {
      int pos = 0;
      char c = '\0';
      char str[256] = "\0";
      while (c != '\n') {
        if (!kbd_keydown())
          continue;
        c = kbd_get();
        if (c == '\b') {
          if (pos != 0) {
            str[--pos] = '\0';
            tty_putchar(c);
          }
        } else if (c == '\n') {
          tty_putchar(c);
        } else {
          str[pos++] = c;
          str[pos] = '\0';
          tty_putchar(c);
        }
      }

      format++;
      switch (*format) {
      case 'c': {

        break;
      }
      case 's': {
        memcpy(va_arg(ap, char *), str, strlen(str) + 1);
        break;
      }
      case 'd': {
        *(va_arg(ap, int *)) = atoi(str);
        break;
      }
      case 'x': {

        break;
      }
      default: {

        break;
      }
      }
      format++;
    }
  }
  va_end(ap);
}
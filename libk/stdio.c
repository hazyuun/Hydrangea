#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include <drivers/kbd.h>
#include <term/term.h>

void printk(const char *format, ...) {
  va_list ap;

  va_start(ap, format);
  while (*format != '\0') {
    if (*format != '%')
      term_putchar(*format++);
    else {
      format++;
      switch (*format) {
      case 'c': {
        term_putchar(va_arg(ap, int));
        break;
      }
      case 's': {
        term_print(va_arg(ap, char *));
        break;
      }
      case 'd': {
        char str[20];
        itoa(va_arg(ap, int), str, 10);
        term_print(str);
        break;
      }
      case 'f': {
        /* Hacky float printing */
        /* TODO: Print floats correctly */
        double f = va_arg(ap, double);
        if (f < 0) {
          term_putchar('-');
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
        term_print(str);
        break;
      }
      default: {
        term_putchar(*(--format));
        term_putchar(*(++format));
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
        uint8_t key_down = kbd_keydown();
        if (key_down == 0xFF)
          continue;
        c = kbd_get();
        if (c == '\b') {
          if (pos != 0) {
            str[--pos] = '\0';
            term_putchar(c);
          }
        } else if (c == '\n') {
          term_putchar(c);
        } else {
          str[pos++] = c;
          str[pos] = '\0';
          term_putchar(c);
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
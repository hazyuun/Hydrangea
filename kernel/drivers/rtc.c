/*
 *      File: rtc.c
 *      Description : TODO
 *
 * */

#include <drivers/rtc.h>
#include <io/io.h>
#include <stdio.h>
#include <string.h>

char *rtc_months[12] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun",
                        "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};

uint8_t rtc_is_ready() {
  io_outb(CMOS_ADDR, 0x0A);
  return !io_inb(CMOS_DATA & 0x80);
}
uint8_t rtc_get_reg(uint32_t reg) {
  io_outb(CMOS_ADDR, reg);
  return io_inb(CMOS_DATA);
}
rtc_time_date_t rtc_now() {
  rtc_time_date_t now;
  memset(&now, 0, sizeof(rtc_time_date_t));
  while (!rtc_is_ready())
    ;
  do {
    now.second = rtc_get_reg(0x00);
    now.minute = rtc_get_reg(0x02);
    now.hour = rtc_get_reg(0x04);
    now.day = rtc_get_reg(0x07);
    now.month = rtc_get_reg(0x08);
    now.year = rtc_get_reg(0x09);
  } while (now.second != rtc_get_reg(0x00) || now.minute != rtc_get_reg(0x02) ||
           now.hour != rtc_get_reg(0x04) || now.day != rtc_get_reg(0x07) ||
           now.month != rtc_get_reg(0x08) || now.year != rtc_get_reg(0x09));
  uint8_t B_reg = rtc_get_reg(0x0B);
  if (!(B_reg & 0x04)) {
    now.second = (now.second & 0x0F) + ((now.second / 16) * 10);
    now.minute = (now.minute & 0x0F) + ((now.minute / 16) * 10);
    now.hour = ((now.hour & 0x0F) + (((now.hour & 0x70) / 16) * 10));
    now.hour |= (now.hour & 0x80);
    now.day = (now.day & 0x0F) + ((now.day / 16) * 10);
    now.month = (now.month & 0x0F) + ((now.month / 16) * 10);
    now.year = (now.year & 0x0F) + ((now.year / 16) * 10);
  }
  if (!(B_reg & 0x02) && (now.hour & 0x80)) {
    now.hour = ((now.hour & 0x7F) + 12) % 24;
  }
  return now;
}
void rtc_print_now() {
  rtc_time_date_t time = rtc_now();

  printk("%d %s %d  %d:%d:%d \n", time.day, rtc_months[time.month - 1],
         time.year, time.hour, time.minute, time.second);
}

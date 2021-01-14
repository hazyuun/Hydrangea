/*
 *      File: rtc.h
 *      Description : TODO
 *
 * */

#ifndef _RTC_H_
#define _RTC_H_

#include <stdint.h>

#define CMOS_ADDR 0x70
#define CMOS_DATA 0x71

typedef struct {
  uint8_t second;
  uint8_t minute;
  uint8_t hour;
  uint8_t day;
  uint8_t month;
  uint8_t century;
  uint32_t year;
} rtc_time_date_t;

uint8_t rtc_is_ready();
uint8_t rtc_get_reg(uint32_t reg);
rtc_time_date_t rtc_now();
void rtc_print_now();

#endif

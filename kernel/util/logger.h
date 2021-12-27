#ifndef _LOGGER_H_
#define _LOGGER_H_

#define INFO NICE_CYAN
#define OKAY NICE_GREEN
#define WARN NICE_YELLOW
#define ERROR NICE_RED

#include <vesa/nice_colors.h>

#include <stdint.h>
void log_f(int64_t lvl, const char *who, char *format, ...);
void log_info(uint32_t color, const char *who, char *format, ...);
void log_result(uint8_t condition, char *format, ...);

#endif
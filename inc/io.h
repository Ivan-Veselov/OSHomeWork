#ifndef __IO_H__
#define __IO_H__

#include <stdint.h>
#include <stdarg.h>

uint64_t printf(const char *format, ...);
uint64_t vprintf(const char *format, va_list arg);

uint64_t snprintf(char *s, uint64_t n, const char *format, ...);
uint64_t vsnprintf(char *s, uint64_t n, const char *format, ...);

#endif /*__IO_H__*/

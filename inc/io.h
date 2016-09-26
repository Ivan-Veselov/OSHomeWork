#ifndef __IO_H__
#define __IO_H__

#include <stdint.h>
#include <stdarg.h>

#define IO_ERR_INVALID_FORMAT -1

int64_t printf(const char *format, ...);
int64_t vprintf(const char *format, va_list arg);

int64_t snprintf(char *s, uint64_t n, const char *format, ...);
int64_t vsnprintf(char *s, uint64_t n, const char *format, va_list arg);

#endif /*__IO_H__*/

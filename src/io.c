#include <io.h>

uint64_t printf(const char *format, ...) {
  va_list arg;
  va_start(arg, format);
  
  uint64_t result = vprintf(format, arg);
  
  va_end(arg);
  
  return result;
}

uint64_t vprintf(const char *format, va_list arg) {
}

uint64_t snprintf(char *s, uint64_t n, const char *format, ...) {
  va_list arg;
  va_start(arg, format);
  
  uint64_t result = vsnprintf(s, n, format, arg);
  
  va_end(arg);
  
  return result;
}

uint64_t vsnprintf(char *s, uint64_t n, const char *format, va_list arg) {
}


#include <io.h>
#include <serial.h>

#define MAX_BITS_NUMBER 64

typedef void (*byte_writer)(uint8_t);
typedef int64_t (*string_writer)(const char*);

static byte_writer write_byte;
static string_writer write_string;

int64_t write_unsigned(uint64_t number, int base) {
  static char str[MAX_BITS_NUMBER + 1];
  int ptr = MAX_BITS_NUMBER - 1;
  
  if (number == 0) {
    str[ptr--] = '0';
  } else {
    while (number) {
      int digit = number % base;
      number /= base;
      
      if (digit < 10) {
        str[ptr--] = '0' + digit;
      } else {
        str[ptr--] = 'a' + digit - 10;
      }
    }
  }
  
  return write_string(str + ptr + 1);
}

int64_t write_signed(int64_t number) {
  int64_t signum_char = 0;
  if (number < 0) {
    write_byte('-');
    signum_char = 1;
    number = ~number + 1;
  }
  
  return signum_char + write_unsigned(number, 10);
}

int64_t printf(const char *format, ...) {
  va_list arg;
  va_start(arg, format);
  
  uint64_t result = vprintf(format, arg);
  
  va_end(arg);
  
  return result;
}

#define NEXT_CHAR(format, i) \
if (format[++i] == '\0') { \
  return IO_ERR_INVALID_FORMAT; \
}

int64_t print(const char *format, va_list arg) {
  uint64_t chars_printed = 0;
  for (uint64_t i = 0; format[i]; ++i) { 
    if (format[i] != '%') {
      write_byte(format[i]);
      ++chars_printed;
      continue;
    }
    
    NEXT_CHAR(format, i);
    
    if (format[i] == '%') {
      write_byte('%');
      ++chars_printed;
      continue;
    }
    
    if (format[i] == 'c') {
      write_byte(va_arg(arg, int)); // promotion
      ++chars_printed;
      continue;
    }
    
    if (format[i] == 's') {
      chars_printed += write_string(va_arg(arg, char*));
      continue;
    }
    
    int h_modifiers = 0, l_modifiers = 0;
    
    int *ptr = &h_modifiers;
    char modifier_letter = 'h';
    
    if (format[i] == 'l') {
      ptr = &l_modifiers;
      modifier_letter = 'l';
    }
    
    while (format[i] == modifier_letter && *ptr < 2) {
      ++(*ptr);
      NEXT_CHAR(format, i);
    }
    
    if (format[i] == 'd' || format[i] == 'i') {
      int64_t number;
      if (modifier_letter == 'h') {
        if (h_modifiers == 0) {
          number = va_arg(arg, int);
        } else if (h_modifiers == 1) {
          number = va_arg(arg, int); // promotion
        } else { // 2
          number = va_arg(arg, int); // promotion
        }
      } else { // 'l'
        if (l_modifiers == 0) {
          number = va_arg(arg, int);
        } else if (l_modifiers == 1) {
          number = va_arg(arg, long);
        } else { // 2
          number = va_arg(arg, long long);
        }
      }
      
      chars_printed += write_signed(number);
      continue;
    }
    
    if (format[i] == 'u' || format[i] == 'o' || format[i] == 'x') {
      uint64_t number;
      if (modifier_letter == 'h') {
        if (h_modifiers == 0) {
          number = va_arg(arg, unsigned);
        } else if (h_modifiers == 1) {
          number = va_arg(arg, int); // promotion
        } else { // 2
          number = va_arg(arg, int); // promotion
        }
      } else { // 'l'
        if (l_modifiers == 0) {
          number = va_arg(arg, unsigned);
        } else if (l_modifiers == 1) {
          number = va_arg(arg, unsigned long);
        } else { // 2
          number = va_arg(arg, unsigned long long);
        }
      }
      
      if (format[i] == 'u') {
        chars_printed += write_unsigned(number, 10);
      } else if (format[i] == 'o') {
        chars_printed += write_unsigned(number, 8);
      } else { // 'x'
        chars_printed += write_unsigned(number, 16);
      }
      
      continue;
    }
    
    return IO_ERR_INVALID_FORMAT;
  }
  
  return chars_printed;
}

int64_t vprintf(const char *format, va_list arg) {
  write_byte = write_byte_to_stdout;
  write_string = write_string_to_stdout;
  
  return print(format, arg);
}

int64_t snprintf(char *s, uint64_t n, const char *format, ...) {
  va_list arg;
  va_start(arg, format);
  
  uint64_t result = vsnprintf(s, n, format, arg);
  
  va_end(arg);
  
  return result;
}

uint64_t dest_str_capacity;
static char *dest_str_ptr;

void write_byte_to_str(uint8_t byte) {
  if (dest_str_capacity) {
    --dest_str_capacity;
    *(dest_str_ptr++) = byte;
  }
}

int64_t write_string_to_str(const char *str) {
  int64_t chars_printed = 0;
  for (int i = 0; str[i]; ++i) {
    write_byte_to_str(str[i]);
    ++chars_printed;
  }
  
  return chars_printed;
}

int64_t vsnprintf(char *s, uint64_t n, const char *format, va_list arg) {
  dest_str_ptr = s;
  dest_str_capacity = n - 1;
  
  write_byte = write_byte_to_str;
  write_string = write_string_to_str;
  
  int64_t chars_printed = print(format, arg);
  s[n - 1 - dest_str_capacity] = '\0';
  return chars_printed;
}


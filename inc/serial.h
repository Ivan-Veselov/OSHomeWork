#ifndef __SERIAL_H__
#define __SERIAL_H__

#include <stdint.h>

void init_serial();
void write_byte_to_stdout(uint8_t);
int64_t write_string_to_stdout(const char*);

#endif /*__SERIAL_H__*/

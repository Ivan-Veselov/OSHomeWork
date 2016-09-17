#include <stdint.h>
#include <ioport.h>
#include <utils.h>

void init_serial() {
  /*
   * Set speed division factor
   */
  
  // Make +0 and +1 ports receive factor
  out8(SERIAL_FST_PORT + 3, bit(7));
  
  uint16_t factor = 1;
  out8(SERIAL_FST_PORT + 0, low_byte(factor));
  out8(SERIAL_FST_PORT + 1, high_byte(factor));
  
  /*
   * Set frame format
   */
   
  uint8_t format = 0;
  format |= bit(0) | bit(1); // 8 data bits in frame
  // last bits are unset
  // 1 stop bit in frame
  // parity check is turned off
  
  out8(SERIAL_FST_PORT + 3, format);
  
  /* 
   * Turn interruptions off
   */
   
  // 7th bit in +3 port must be unset
  out8(SERIAL_FST_PORT + 1, 0);
  
  write_string_to_stdout("Serial Interface is initialized\n");
}

void write_byte_to_stdout(uint8_t byte) {
  // wait until previous write operation ends
  while (!get_bit(in8(SERIAL_FST_PORT + 5), 5));
  
  out8(SERIAL_FST_PORT + 0, byte);
}

void write_string_to_stdout(const char *str) {
  for (int i = 0; str[i]; ++i) {
    write_byte_to_stdout(str[i]);
  }
}


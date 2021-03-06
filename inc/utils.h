#ifndef __UTILS_H__
#define __UTILS_H__

#include <stdint.h>

#define bit(b) (1 << (b))
#define get_bit(number, bit) ((number) & (1 << (bit)))
#define set_bit(number, bit) ((number) | (1 << (bit)))
#define rem_bit(number, bit) ((number) & (~(1 << (bit))))

#define low_byte(n) ((n) & 0xffu)
#define high_byte(n) (((n) >> 8) & 0xffu)
#define low_word(n) ((n) & 0xffffu)
#define high_word(n) (((n) >> 16) & 0xffffu)
#define low_dword(n) ((n) & 0xffffffffull)
#define high_dword(n) (((n) >> 32) & 0xffffffffull)

static inline uint64_t u64max(uint64_t a, uint64_t b) {
  if (a > b) {
    return a;
  } else {
    return b;
  }
}

static inline void swap(void *a, void *b, uint64_t size) {
  uint8_t *_a = a;
  uint8_t *_b = b;
  
  for (uint64_t i = 0; i < size; ++i, ++_a, ++_b) {
    uint8_t tmp = *_a;
    *_a = *_b;
    *_b = tmp;
  }
}

static inline uint64_t next_or_this_power_of_2(uint64_t number) {
  uint64_t res = 1;
  while (res < number) {
    res <<= 1;
  }
  
  return res;
}

static inline uint8_t next_or_this_power_of_2_exp(uint64_t number) {
  uint8_t res = 0;
  uint64_t power = 1;
  while (power < number) {
    power <<= 1;
    ++res;
  }
  
  return res;
}

#endif /*__UTILS_H__*/

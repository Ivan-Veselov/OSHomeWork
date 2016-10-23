#ifndef __UTILS_H__
#define __UTILS_H__

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

static uint64_t u64min(uint64_t a, uint64_t b) {
  if (a < b) {
    return a;
  } else {
    return b;
  }
}

#endif /*__UTILS_H__*/

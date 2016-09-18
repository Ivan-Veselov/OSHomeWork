#ifndef __UTILS_H__
#define __UTILS_H__

#define bit(b) (1 << (b))
#define get_bit(number, bit) ((number) & (1 << (bit)))
#define set_bit(number, bit) ((number) | (1 << (bit)))
#define rem_bit(number, bit) ((number) & (~(1 << (bit))))

#define low_byte(n) ((n) & 0x00ffu)
#define high_byte(n) (((n) & 0xff00u) >> 8)
#define low_word(n) ((n) & 0x0000ffffu)
#define high_word(n) (((n) & 0xffff0000u) >> 16)
#define low_dword(n) ((n) & 0x00000000ffffffffull)
#define high_dword(n) (((n) & 0xffffffff00000000ull) >> 32)

#endif /*__UTILS_H__*/

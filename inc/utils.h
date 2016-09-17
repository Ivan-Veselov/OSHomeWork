#ifndef __UTILS_H__
#define __UTILS_H__

#define bit(b) (1 << (b))
#define get_bit(number, bit) ((number) & (1 << (bit)))
#define set_bit(number, bit) ((number) | (1 << (bit)))
#define rem_bit(number, bit) ((number) & (~(1 << (bit))))

#define low_byte(n) ((n) & 0x00ffu)
#define high_byte(n) ((n) & 0xff00u)

#endif /*__UTILS_H__*/

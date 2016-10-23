#ifndef __MEMORY_MAP_H__
#define __MEMORY_MAP_H__

#include <stdint.h>

struct memory_block {
  uint64_t base_addr;
  uint64_t length;
  uint32_t type;
};

typedef struct memory_block memory_block_t;

#define MAX_BLOCKS_IN_MEMMAP 32
#define RESERVED_BLOCK 2

extern struct memory_block memory_map[];
extern uint64_t memory_map_size;

void init_memory_map(uint64_t boot_info_ptr);
void print_memory_block(struct memory_block *block);
void print_memory_map();

#endif /*__MEMORY_MAP_H__*/

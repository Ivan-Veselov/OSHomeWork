#ifndef __BUDDY_ALLOC_H__
#define __BUDDY_ALLOC_H__

#include <stdint.h>
#include <stddef.h>
#include <memory.h>
#include <memory_map.h>

void init_buddy_allocator();
void* buddy_alloc_by_order(uint8_t order);
void* buddy_alloc(uint64_t size);
void  buddy_free(void* addr);

uint64_t get_pages_allocated();

#endif /*__BUDDY_ALLOC_H__*/

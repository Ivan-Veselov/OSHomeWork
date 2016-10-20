#ifndef __SLAB_ALLOC_H__
#define __SLAB_ALLOC_H__

#include <stdint.h>
#include <stddef.h>
#include <buddy_alloc.h>

typedef uintptr_t* slab_t;

slab_t init_slab(uint64_t unit_size, uint64_t units_number);
void destroy_slab(slab_t slab);

void* slab_alloc(slab_t slab);
void slab_free(slab_t slab, void* addr);

#endif /*__SLAB_ALLOC_H__*/

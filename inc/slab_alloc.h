#ifndef __SLAB_ALLOC_H__
#define __SLAB_ALLOC_H__

#include <stdint.h>
#include <stddef.h>
#include <buddy_alloc.h>

struct slab_unit {
  struct slab_unit *next;
};

typedef struct slab_unit slab_unit_t;

struct slab_allocator {
  slab_unit_t *head;
};

typedef struct slab_allocator slab_allocator_t;

slab_allocator_t* init_slab(uint64_t unit_size, uint64_t units_number);
void destroy_slab(slab_allocator_t *allocator);

void* slab_alloc(slab_allocator_t *allocator);
void slab_free(slab_allocator_t *allocator, void *addr);

#endif /*__SLAB_ALLOC_H__*/

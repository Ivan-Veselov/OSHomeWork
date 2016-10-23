#include <slab_alloc.h>
#include <io.h>
#include <utils.h>

uint64_t next_or_this_power_of_2(uint64_t number) {
  uint64_t res = 1;
  while (res < number) {
    res <<= 1;
  }
  
  return res;
}

slab_t init_slab(uint64_t unit_size, uint64_t units_number) {
  unit_size = u64min(sizeof(void*), unit_size);
  
  uint64_t bytes_needed = unit_size * units_number + sizeof(void*);
  uint64_t pages = (bytes_needed + PAGE_SIZE - 1) / PAGE_SIZE;
  
  slab_t memory = (slab_t)buddy_alloc(next_or_this_power_of_2(pages));
  if (memory == NULL) {
    return NULL;
  }
  
  uintptr_t *ptr = memory;
  *ptr = (uintptr_t)ptr + sizeof(void*);
  ptr = (uintptr_t*)(*ptr);
  
  for (uint64_t i = 0; i < units_number; ++i, ptr = (uintptr_t*)(*ptr)) {
    *ptr = (uintptr_t)ptr + unit_size;
  }
  
  *ptr = 0;
  
  return memory;
}

void destroy_slab(slab_t slab) {
  buddy_free(slab);
}

void* slab_alloc(slab_t slab) {
  if (*slab == 0) {
    return NULL;
  }
  
  uintptr_t *ptr = (uintptr_t*)(*slab);
  *slab = *((uintptr_t*)(*slab));
  
  return ptr;
}

void slab_free(slab_t slab, void* addr) {
  *((uintptr_t*)addr) = *slab;
  *slab = (uintptr_t)addr;
}


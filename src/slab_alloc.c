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

slab_allocator_t* init_slab(uint64_t unit_size, uint64_t units_number) {
  if (units_number == 0) {
    return NULL;
  }
  
  unit_size = u64min(sizeof(slab_unit_t), unit_size);
  
  uint64_t bytes_needed = unit_size * units_number + sizeof(slab_allocator_t);
  uint64_t pages = (bytes_needed + PAGE_SIZE - 1) / PAGE_SIZE;
  
  void* memory = buddy_alloc(next_or_this_power_of_2(pages));
  if (memory == NULL) {
    return NULL;
  }
  
  slab_allocator_t *allocator = (slab_allocator_t*)memory;
  slab_unit_t *unit = allocator->head = (slab_unit_t*)((uintptr_t)memory + sizeof(slab_allocator_t));
  
  for (uint64_t i = 0; i < units_number; ++i, unit = unit->next) {
    unit->next = (slab_unit_t*)((uintptr_t)unit + unit_size);
  }
  
  unit->next = NULL;
  
  return allocator;
}

void destroy_slab(slab_allocator_t *allocator) {
  buddy_free(allocator);
}

void* slab_alloc(slab_allocator_t *allocator) {
  if (allocator->head == NULL) {
    return NULL;
  }
  
  slab_unit_t *unit = allocator->head;
  allocator->head = unit->next;
  
  return unit;
}

void slab_free(slab_allocator_t *allocator, void *addr) {
  slab_unit_t *unit = (slab_unit_t*)addr;
  unit->next = allocator->head;
  allocator->head = unit;
}


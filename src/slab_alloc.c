#include <slab_alloc.h>
#include <io.h>
#include <utils.h>

slab_allocator_t* init_slab(uint64_t unit_size, uint64_t units_number) {
  if (units_number == 0) {
    return NULL;
  }
  
  unit_size = u64min(sizeof(slab_unit_t), unit_size);
  
  uint64_t bytes_needed = unit_size * units_number + sizeof(slab_allocator_t);

  void* memory = buddy_alloc(bytes_needed);
  if (memory == NULL) {
    return NULL;
  }
  
  slab_allocator_t *allocator = (slab_allocator_t*)memory;
  
  allocator->next = NULL;
  allocator->allocated_units = 0;
  slab_unit_t *unit = allocator->head = (slab_unit_t*)((uintptr_t)memory + sizeof(slab_allocator_t));
  
  for (uint64_t i = 0; i < units_number - 1; ++i, unit = unit->next) {
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
  allocator->allocated_units++;
  
  return unit;
}

void slab_free(slab_allocator_t *allocator, void *addr) {
  slab_unit_t *unit = (slab_unit_t*)addr;
  unit->next = allocator->head;
  allocator->head = unit;
  allocator->allocated_units--;
}


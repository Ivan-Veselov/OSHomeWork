#include <malloc.h>
#include <memory.h>
#include <buddy_alloc.h>
#include <slab_alloc.h>
#include <utils.h>

#define BIG_SIZE (PAGE_SIZE / 8)
#define MIN_SIZE 16
#define SIZE_TYPE_NUM 6 /* 16, 32, 64, 128, 256, 512 */

struct memory_info {
  slab_allocator_t *slab_allocator;
  uint8_t size_type;
};

typedef struct memory_info memory_info_t;

slab_allocator_t* slab_list_head[SIZE_TYPE_NUM];

void* write_info(void *memory, slab_allocator_t *allocator, uint8_t size_type) {
  memory_info_t *info = (memory_info_t*)memory;
  info->slab_allocator = allocator;
  info->size_type = size_type;
  return memory;
}

void* allocate_with_slab(uint8_t size_type) {
  slab_allocator_t **head_allocator = slab_list_head + size_type;
  
  if (head_allocator == NULL) {
    uint64_t size = MIN_SIZE << size_type;
    *head_allocator = init_slab(size, (PAGE_SIZE - sizeof(slab_allocator_t)) / size);
    if (head_allocator == NULL) {
      return NULL;  
    }
  }
  
  void *memory = slab_alloc(*head_allocator);
  
  if ((*head_allocator)->head == NULL) {
    *head_allocator = (*head_allocator)->next;
    (*head_allocator)->next = NULL;
  }
  
  return write_info(memory, *head_allocator, size_type);
}

void* malloc(uint64_t size) {
  size += sizeof(memory_info_t);
  
  if (size >= BIG_SIZE) {
    void *memory = buddy_alloc(size);
    if (memory == NULL) {
      return NULL;
    }
    
    return write_info(memory, NULL, 0);
  }

  size = next_or_this_power_of_2(size);
  uint8_t size_type = 0;
  while (size >>= 1) {
    ++size_type;
  }

  return allocate_with_slab(size_type);
}

void free_slab(memory_info_t *info, void *addr) {
  slab_allocator_t *allocator = info->slab_allocator;
  slab_allocator_t **head_allocator = slab_list_head + info->size_type;
  if (allocator->head == NULL) {
    allocator->next = *head_allocator;
    *head_allocator = allocator;
  }
  
  slab_free(allocator, addr);
  
  if (allocator->allocated_units == 0) {
    *head_allocator = allocator->next;
    destroy_slab(allocator);
  }
}

void free(void *addr) {
  memory_info_t *info = (memory_info_t*)addr;
  if (info->slab_allocator == NULL) {
    buddy_free(addr);
    return;
  }
  
  free_slab(info, addr);
}


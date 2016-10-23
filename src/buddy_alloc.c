#include <buddy_alloc.h>
#include <io.h>
#include <utils.h>

#define FIRST_LOGICAL_ADDR 0xffff800000000000ull
#define LOGICAL_ADDR(phys_addr) ((phys_addr) + FIRST_LOGICAL_ADDR)
#define PHYSICAL_ADDR(log_addr) ((log_addr) - FIRST_LOGICAL_ADDR)

struct page_descriptor;

typedef struct page_descriptor page_descriptor_t;

struct pages_region {
  uint64_t size;
  page_descriptor_t *descriptors;
  
  uint64_t first_page_logical_addr;
};

typedef struct pages_region pages_region_t;

struct page_descriptor {
  uint8_t allocated;
  uint8_t order;

  pages_region_t *region;
  
  struct page_descriptor *prev;
  struct page_descriptor *next;
};

uint64_t get_id(page_descriptor_t *desc) {
  return desc - desc->region->descriptors;
}

page_descriptor_t* get_buddy(page_descriptor_t *desc) {
  uint64_t this_id = get_id(desc);
  uint64_t buddy_id = this_id ^ (1ll << desc->order);
  
  if (buddy_id >= desc->region->size) {
    return NULL;
  }
  
  page_descriptor_t *buddy_desc = desc->region->descriptors + buddy_id;
  if (buddy_desc->allocated || buddy_desc->order != desc->order) {
    return NULL;
  }
  
  return buddy_desc;
}

void* get_page_logical_addr(page_descriptor_t *desc) {
  return (void*)(desc->region->first_page_logical_addr + PAGE_SIZE * get_id(desc));
}

pages_region_t* memory_block_region[MAX_BLOCKS_IN_MEMMAP];

page_descriptor_t* get_page_descriptor_by_addr(void *addr) {
  uint64_t int_addr = PHYSICAL_ADDR((uint64_t)addr);
  for (uint64_t i = 0; i < memory_map_size; ++i) {
    memory_block_t *block = memory_map + i;
    if (!(block->base_addr <= int_addr && int_addr < block->base_addr + block->length)) {
      continue;
    }
    
    pages_region_t *region = memory_block_region[i];
    return region->descriptors + (int_addr - PHYSICAL_ADDR(region->first_page_logical_addr)) / PAGE_SIZE;
  }
  
  return NULL;
}

#define MAX_ORDER_NUM 64
page_descriptor_t* desc_list_head[MAX_ORDER_NUM];

void erase(page_descriptor_t *desc) {
  if (desc->prev) {
    desc->prev->next = desc->next;
  }
  
  if (desc->next) {
    desc->next->prev = desc->prev;
  }
  
  if (desc->prev == NULL) {
    desc_list_head[desc->order] = desc->next;
  }
  
  desc->prev = NULL;
  desc->next = NULL;
}

void insert(page_descriptor_t *desc) {
  desc->next = desc_list_head[desc->order];
  desc_list_head[desc->order] = desc;
  if (desc->next) {
    desc->next->prev = desc;
  }
}

page_descriptor_t* promote(page_descriptor_t *desc) {
  if (desc->allocated) {
    return NULL;
  }
 
  page_descriptor_t *buddy_desc = get_buddy(desc);
  if (buddy_desc == NULL) {
    return NULL;
  }
  
  erase(desc);
  erase(buddy_desc);

  if (buddy_desc < desc) {
    desc = buddy_desc;
  }
  
  desc->order++;
  insert(desc);
  return desc;
}

page_descriptor_t* demote(page_descriptor_t *desc) {
  if (desc->allocated || desc->order == 0) {
    return NULL;
  }
  
  erase(desc);
  
  desc->order--;
  insert(desc);
  insert(get_buddy(desc));
  
  return desc;
}

void* buddy_alloc_by_order(uint8_t order) {
  for (uint8_t i = order; i < MAX_ORDER_NUM; ++i) {
    if (desc_list_head[i] == NULL) {
      continue;
    }
    
    page_descriptor_t *desc = desc_list_head[i];
    while (desc->order > order) {
      desc = demote(desc);
    }
    
    erase(desc);
    desc->allocated = 1;
    return get_page_logical_addr(desc);
  }
  
  return NULL;
}

void* buddy_alloc(uint64_t size) {
  uint64_t pages = (size + PAGE_SIZE - 1) / PAGE_SIZE;
  return buddy_alloc_by_order(next_or_this_power_of_2_exp(pages));
}

void buddy_free(void* addr) {
  page_descriptor_t *desc = get_page_descriptor_by_addr(addr);
  
  desc->allocated = 0;
  insert(desc);
  while ((desc = promote(desc)));
}

uint64_t add_memory_block_to_buddy(memory_block_t *block, uint64_t block_id) {
  uint64_t first_page_addr = block->base_addr;
  if (first_page_addr % PAGE_SIZE != 0) {
    first_page_addr += PAGE_SIZE - (first_page_addr % PAGE_SIZE);
  }
  
  uint64_t block_size = block->length - (first_page_addr - block->base_addr);
  uint64_t pages_amount = (block_size - sizeof(pages_region_t)) / (PAGE_SIZE + sizeof(page_descriptor_t));
  
  if (pages_amount < 1) {
    return 0;
  }
  
  first_page_addr = LOGICAL_ADDR(first_page_addr);
  page_descriptor_t *first_descriptor = (page_descriptor_t*)(first_page_addr + PAGE_SIZE * pages_amount);
  
  pages_region_t *region = (pages_region_t*)(first_descriptor + pages_amount);
  region->size = pages_amount;
  region->descriptors = first_descriptor;
  region->first_page_logical_addr = first_page_addr;
  
  for (uint64_t i = 0; i < region->size; ++i) {
    region->descriptors[i].allocated = 0;
    region->descriptors[i].order = 0;
    region->descriptors[i].region = region;
    
    region->descriptors[i].next = NULL;
    region->descriptors[i].prev = NULL;
    
    insert(region->descriptors + i);
  }
  
  memory_block_region[block_id] = region;
  return pages_amount;
}

void init_buddy_allocator() {
  for (uint64_t i = 0; i < MAX_ORDER_NUM; ++i) {
    desc_list_head[i] = NULL;
  }
  
  uint64_t pages_amount = 0;
  for (uint64_t i = 0; i < memory_map_size; ++i) {
    memory_block_t *block = memory_map + i;
    if (block->type == RESERVED_BLOCK) {
      continue;
    }
    
    pages_amount += add_memory_block_to_buddy(block, i);
  }
  
  printf("%d pages will be available for buddy allocator\n", pages_amount);
  
  for (uint64_t i = 0; i < (uint64_t)(MAX_ORDER_NUM - 1); ++i) {
    page_descriptor_t *head = NULL;
    while (desc_list_head[i]) {
      if (promote(desc_list_head[i]) == NULL) {
        page_descriptor_t *desc = desc_list_head[i];
        erase(desc);
        
        desc->next = head;
        if (head) {
          head->prev = desc;
        }
        
        head = desc;
      }
    }
    
    desc_list_head[i] = head;
  }
  
  printf("Buddy allocator is initialized\n");
}


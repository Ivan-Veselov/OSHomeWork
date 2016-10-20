#include <buddy_alloc.h>
#include <io.h>

#define FIRST_LOGICAL_ADDR 0xffff800000000000ll
#define LOGICAL_ADDR(phys_addr) ((phys_addr) + VIRTUAL_BASE)
#define PHYSICAL_ADDR(log_addr) ((log_addr) - VIRTUAL_BASE)

struct page_descriptor {
  uint8_t allocated;
  uint8_t order;
  
  struct page_descriptor *prev;
  struct page_descriptor *next;
};

uint64_t first_page_addr;

#define MAX_ORDER_NUM 64
struct page_descriptor* desc_list_head[MAX_ORDER_NUM];
uint8_t order_amount;

uint64_t page_amount;
struct page_descriptor* desc_array;

#define PAGE_ID(addr) ((addr) - desc_array)
#define BUDDY(page) (desc_array + (PAGE_ID(page) ^ (1ll << (page)->order)))
#define PAGE_BEGIN(page) LOGICAL_ADDR(first_page_addr + PAGE_SIZE * PAGE_ID(page))
#define PAGE_FROM_BEGINNING(addr) (desc_array + ((PHYSICAL_ADDR(addr) - first_page_addr) / PAGE_SIZE))

void erase(struct page_descriptor *desc) {
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

void insert(struct page_descriptor *desc) {
  desc->next = desc_list_head[desc->order];
  desc_list_head[desc->order] = desc;
  if (desc->next) {
    desc->next->prev = desc;
  }
}

struct page_descriptor* promote(struct page_descriptor *page) {
  if (page->allocated) {
    return NULL;
  }
 
  struct page_descriptor *buddy = BUDDY(page);
  if ((uint64_t)PAGE_ID(buddy) >= page_amount || buddy->allocated || buddy->order != page->order) {
    return NULL;
  }
  
  erase(page);
  erase(buddy);

  if (buddy < page) {
    page = buddy;
  }
  
  page->order++;
  insert(page);
  return page;
}

struct page_descriptor* demote(struct page_descriptor *page) {
  if (page->allocated || page->order == 0) {
    return NULL;
  }
  
  page->order--;
  insert(page);
  insert(BUDDY(page));
  
  return page;
}

void* buddy_alloc(uint8_t order) {
  for (uint8_t i = order; i < order_amount; ++i) {
    if (desc_list_head[i] == NULL) {
      continue;
    }
    
    struct page_descriptor *page = desc_list_head[i];
    while (page->order > order) {
      page = demote(page);
    }
    
    erase(page);
    page->allocated = 1;
    return (void*)PAGE_BEGIN(page);
  }
  
  return NULL;
}

void buddy_free(void* addr) {
  struct page_descriptor *page = PAGE_FROM_BEGINNING((uint64_t)addr);
  
  page->allocated = 0;
  insert(page);
  while ((page = promote(page)));
}

struct memory_block* find_max_memory_block() {
  struct memory_block *max_block = NULL;
  uint64_t max_block_len = 0;
  
  for (struct memory_block *block = memory_map;
       block != memory_map + memory_map_size; ++block) {
    if (block->type == RESERVED_BLOCK) {
      continue;
    }
    
    if (block->length > max_block_len) {
      max_block = block;
      max_block_len = block->length;
    }
  }
  
  return max_block;
}

void init_buddy_allocator() {
  struct memory_block *memory_block = find_max_memory_block();
  if (memory_block == NULL) {
    printf("Unable to find free memory block for buddy allocator!\n");
    return;
  }
  
  printf("Found a block for buddy allocator: ");
  print_memory_block(memory_block);
  
  first_page_addr = memory_block->base_addr;
  if (first_page_addr % PAGE_SIZE != 0) {
    first_page_addr += PAGE_SIZE - (first_page_addr % PAGE_SIZE);
  }
  
  uint64_t block_size = memory_block->length - (first_page_addr - memory_block->base_addr);
  page_amount = block_size / (PAGE_SIZE + sizeof(struct page_descriptor));
  
  if (page_amount < 1) {
    printf("Block size is too small to create buddy allocator on it!");
    return;
  }
  
  printf("%d pages will be available\n", page_amount);
  desc_array = (struct page_descriptor*)(LOGICAL_ADDR(first_page_addr + page_amount * PAGE_SIZE));
  
  for (uint64_t i = 0; i < page_amount; ++i) {
    desc_array[i].allocated = 0;
    desc_array[i].order = 0;
    desc_array[i].prev = desc_array + (i - 1);
    desc_array[i].next = desc_array + (i + 1);
  }
  
  desc_array[0].prev = NULL;
  desc_array[page_amount - 1].next = NULL;
  
  for (uint64_t i = 0; i < MAX_ORDER_NUM; ++i) {
    desc_list_head[i] = NULL;
  }
  
  desc_list_head[0] = desc_array;
  
  order_amount = 0;
  while ((1llu << order_amount) <= page_amount) {
    ++order_amount;
  }
  
  for (uint64_t i = 0; i < (uint64_t)(order_amount - 1); ++i) {
    struct page_descriptor *head = NULL;
    while (desc_list_head[i]) {
      if (promote(desc_list_head[i]) == NULL) {
        head = desc_list_head[i];
        erase(head);
      }
    }
    
    desc_list_head[i] = head;
  }
  
  printf("Buddy allocator is initialized\n");
}


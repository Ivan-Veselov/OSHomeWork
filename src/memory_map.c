#include <memory_map.h>
#include <io.h>
#include <utils.h>
#include <stddef.h>
#include <memory.h>

typedef uint8_t* pointer;

memory_block_t memory_map[MAX_BLOCKS_IN_MEMMAP];
uint64_t memory_map_size = 0;

extern char text_phys_begin[];
extern char bss_phys_end[];

struct mmap_entry {
  uint32_t size;
  uint64_t base_addr;
  uint64_t length;
  uint32_t type;
} __attribute__((packed));

void add_memory_block(uint64_t base_addr, uint64_t length, uint32_t type) {
  memory_block_t *memory_block = memory_map + memory_map_size++;
  
  memory_block->base_addr = base_addr;
  memory_block->length = length;
  memory_block->type = type;
}

void sort_memory_map() {
  for (uint64_t i = 0; i < memory_map_size; ++i) {
    for (uint64_t j = i + 1; j < memory_map_size; ++j) {
      if (memory_map[i].base_addr > memory_map[j].base_addr) {
        swap(memory_map + i, memory_map + j, sizeof(memory_block_t));
      }
    }
  }
}

extern uint32_t pml4_i;
void init_paging() {
  memory_block_t *block = NULL;
  for (uint64_t i = 0; i < memory_map_size; ++i) {
    block = memory_map + i;
    if (block->type != RESERVED_BLOCK && block->length >= PAGE_SIZE) {
      break;
    }
  }
  
  if (block == NULL) {
    printf("Unable to find memory for PDPT!\n");
    return;
  }
  
  //uint64_t *pdpt = (uint64_t*)block->base_addr;
  if (block->length == PAGE_SIZE) {
    block->type = RESERVED_BLOCK;
  } else {
    add_memory_block(block->base_addr, PAGE_SIZE, RESERVED_BLOCK);
    block->base_addr += PAGE_SIZE;
    block->length -= PAGE_SIZE;
    sort_memory_map();
  }
  
  //printf("pml4: 0x%llx\n", pml4_i);
  // ???...
  /*uint64_t *pml4 = (uint64_t*)(uint64_t)pml4_i;
  pml4 = pml4;
  pdpt = pdpt;
  
  uint64_t *ptr = NULL;
  *ptr = 5;*/
}

void init_memory_map(uint64_t boot_info_ptr) {
  memory_block_t *kernel_block = memory_map + (memory_map_size++);
  kernel_block->base_addr = (uint64_t)text_phys_begin;
  kernel_block->length = (uint64_t)bss_phys_end - (uint64_t)text_phys_begin;
  kernel_block->type = RESERVED_BLOCK;
  
  uint64_t kernel_left_border = kernel_block->base_addr;
  uint64_t kernel_right_border = kernel_block->base_addr + kernel_block->length - 1;
  
  pointer boot_info = (pointer)boot_info_ptr;
  
  uint32_t mmap_length = *(boot_info + 44);
  pointer mmap_addr = (pointer)(*(uint64_t*)(boot_info + 48));
  
  pointer mmap_ptr = mmap_addr;
  while (mmap_ptr < mmap_addr + mmap_length) {
    struct mmap_entry *mmap_entry = (struct mmap_entry*)mmap_ptr;
    mmap_ptr += sizeof(mmap_entry->size) + mmap_entry->size;
    
    uint64_t block_left_border = mmap_entry->base_addr;
    uint64_t block_right_border = mmap_entry->base_addr + mmap_entry->length - 1;
    
    if (kernel_right_border < block_left_border ||
                              block_right_border < kernel_left_border) {
      add_memory_block(mmap_entry->base_addr,
                       mmap_entry->length,
                       mmap_entry->type);
      continue;
    }
    
    if (block_left_border < kernel_left_border) {
      add_memory_block(block_left_border,
                       kernel_left_border - block_left_border,
                       mmap_entry->type);
    }
    
    if (kernel_right_border < block_right_border) {
      add_memory_block(kernel_right_border + 1,
                       block_right_border - kernel_right_border,
                       mmap_entry->type);
    }
  }
  
  sort_memory_map();
  
  printf("Memory map is initialized\n");
  
  init_paging();
}

void print_memory_block(struct memory_block *block) {
  printf("memory block: 0x%llx-0x%llx (%d bytes), type %d\n",
           block->base_addr,
           block->base_addr + block->length - 1,
           block->length,
           block->type);
}

void print_memory_map() {
  printf("Memory map:\n");
  printf("----------------\n");
  for (uint64_t i = 0; i < memory_map_size; ++i) {
    print_memory_block(memory_map + i);
  }
  
  printf("----------------\n");
}


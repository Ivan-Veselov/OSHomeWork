#include <memory_map.h>
#include <io.h>

typedef uint8_t* pointer;

#define MAX_BLOCKS_IN_MEMMAP 32

struct memory_block memory_map[MAX_BLOCKS_IN_MEMMAP];
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
  struct memory_block *memory_block = memory_map + memory_map_size++;
  
  memory_block->base_addr = base_addr;
  memory_block->length = length;
  memory_block->type = type;
}

void init_memory_map(uint64_t boot_info_ptr) {
  struct memory_block *kernel_block = memory_map + (memory_map_size++);
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
  
  printf("Memory map is initialized\n");
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


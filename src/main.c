static void qemu_gdb_hang(void) {
#ifdef DEBUG
	static volatile int wait = 1;

	while (wait);
#endif
}

#include <desc.h>
#include <serial.h>
#include <idt.h>
#include <int_controller.h>
#include <timer.h>
#include <io.h>
#include <memory_map.h>
#include <buddy_alloc.h>
#include <slab_alloc.h>
#include <malloc.h>

extern uint32_t boot_info;

void main(void) {
	qemu_gdb_hang();

  init_serial();
  init_idt();
  init_int_controller();
  init_memory_map(boot_info);
  print_memory_map();
  init_buddy_allocator();
  
  /* Allocators testing */

  for (uint64_t test = 0; test < 10; ++test) {
    const uint64_t size = 1000;
    
    uint64_t ***array = (uint64_t***)malloc(sizeof(uint64_t**) * size);
    for (uint64_t i = 0; i < size; ++i) {
      array[i] = (uint64_t**)malloc(sizeof(uint64_t*) * size);
      for (uint64_t j = 0; j < size; ++j) {
        array[i][j] = (uint64_t*)malloc(sizeof(uint64_t));
      }
    }
    
    for (uint64_t i = 0; i < size; ++i) {
      for (uint64_t j = 0; j < size; ++j) {
        *(array[i][j]) = i * size + j;
      }
    }
    
    for (uint64_t i = 0; i < size; ++i) {
      for (uint64_t j = 0; j < size; ++j) {
        free(array[i][j]);
      }
      
      free(array[i]);
    }
    
    free(array);
  }

  printf("Pages allocated: %llu\n", get_pages_allocated());
  
  /*____________________*/

  printf("Main hang...\n");  
	while (1);
}


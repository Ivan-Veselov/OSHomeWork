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

  for (uint64_t test = 0; test < 1000; ++test) {
    const uint64_t size = 100;
    
    uint64_t **array = (uint64_t**)malloc(sizeof(uint64_t*) * size);
    
    for (uint64_t i = 0; i < size; ++i) {
      array[i] = (uint64_t*)malloc(sizeof(uint64_t));
    }
    
    for (uint64_t i = 0; i < size; ++i) {
      *(array[i]) = i;
    }
    
    for (uint64_t i = 0; i < size; ++i) {
      free(array[i]);
    }
    
    free(array);
  }
  
  /*____________________*/
  
	while (1);
}


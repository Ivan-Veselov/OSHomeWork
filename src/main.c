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
    uint64_t *array = (uint64_t*)malloc(sizeof(uint64_t) * 100);
    for (uint64_t i = 0; i < 100; ++i) {
      array[i] = i;
    }
    
    free(array);
    
    uint64_t *number = malloc(sizeof(uint64_t));
    *number = 42;
    free(number);
  }
  
  /*____________________*/
  
	while (1);
}


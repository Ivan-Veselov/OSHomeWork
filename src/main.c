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
#include <threads.h>

extern uint32_t boot_info;

void run(void *arg) {
  for (int i = 0; i < 10000; ++i) {
    thread_lock();
    printf("Thread %d: %d\n", *((int*)arg), i);
    thread_unlock();
  }
}

void main(void) {
	qemu_gdb_hang();

  init_serial();
  init_idt();
  init_int_controller();
  init_memory_map(boot_info);
  print_memory_map();
  init_buddy_allocator();
  init_thread_system();
  init_timer(TIMER_MODE_RATE_GENERATOR, 0xffffu);

  int n1 = 1;
  int n2 = 2;
  
  thread_t *thread1 = thread_create(run, &n1);
  thread_t *thread2 = thread_create(run, &n2);
  thread_join(thread1);
  thread_join(thread2);

  printf("Main hang...\n");  
	while (1);
}


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

extern uint32_t boot_info;

void main(void) {
	qemu_gdb_hang();

  init_serial();
  init_idt();
  init_int_controller();
  init_memory_map(boot_info);
  print_memory_map();

	while (1);
}

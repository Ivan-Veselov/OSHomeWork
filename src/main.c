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

void main(void) {
	qemu_gdb_hang();

  init_serial();
  init_idt();
  init_int_controller();

	while (1);
}

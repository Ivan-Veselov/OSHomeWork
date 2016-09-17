static void qemu_gdb_hang(void) {
#ifdef DEBUG
	static volatile int wait = 1;

	while (wait);
#endif
}

#include <desc.h>
#include <serial.h>

void main(void) {
	qemu_gdb_hang();

  init_serial();
  write_string_to_stdout("Serial Interface is initialized\n");

	struct desc_table_ptr ptr = {0, 0};
	write_idtr(&ptr);

	while (1);
}

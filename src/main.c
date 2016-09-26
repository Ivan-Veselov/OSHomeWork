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

  // test
  __asm__("int $0");
  
  char buffer[5];
  
  if (printf("0x%x\n", 255) == 5) {
    if (printf("%d\n", -300) == 5) {
      if (printf("%d\n", 0) == 2) {
        if (snprintf(buffer, 5, "%d+%d", 250, 250) == 7) {
          printf("%s\n", buffer);
        }
      }
    }
  }
  
  init_timer(TIMER_MODE_RATE_GENERATOR, 0xffffu);
  
	while (1);
}

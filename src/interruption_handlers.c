#include <stdint.h>
#include <serial.h>
#include <int_controller.h>
#include <io.h>
#include <threads.h>

void master_interruption_handler(uint64_t interruption_id) {
  if (interruption_id == IDT_FIRST_MASTER_DESCRIPTOR) {
    thread_schedule();
  }
  
  end_of_interrupt_master();
}

void slave_interruption_handler(uint64_t interruption_id) {
  (void)interruption_id;
  
  // handling

  end_of_interrupt_slave();
  end_of_interrupt_master();
}

void interruption_handler(uint64_t interruption_id) {
  if (IDT_FIRST_MASTER_DESCRIPTOR <= interruption_id &&
                  interruption_id <= IDT_LAST_MASTER_DESCRIPTOR)
  {
    master_interruption_handler(interruption_id);
    return;
  }
  
  if (IDT_FIRST_SLAVE_DESCRIPTOR <= interruption_id &&
                  interruption_id <= IDT_LAST_SLAVE_DESCRIPTOR)
  {
    slave_interruption_handler(interruption_id);
    return;
  }
  
  printf("Interruption %d\n", interruption_id);
}


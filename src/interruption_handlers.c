#include <stdint.h>
#include <serial.h>
#include <int_controller.h>

void master_interruption_handler(uint64_t interruption_id) {
  // handling
  
  // supress warning
  interruption_id = interruption_id + 1;
  
  end_of_interrupt_master();
}

void slave_interruption_handler(uint64_t interruption_id) {
  // handling

  // supress warning
  interruption_id = interruption_id + 1;

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
  
  if (interruption_id == 0) {
    write_string_to_stdout("Interruption 0\n");
  } else {
    write_string_to_stdout("...\n");
  }
}


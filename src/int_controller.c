#include <int_controller.h>
#include <ioport.h>
#include <serial.h>
#include <utils.h>

void init_int_controller() {
  /*
   * Initialize master controller
   */
  
  /*
   * 0 -- 3 bytes of data
   * 1 -- cascade of controllers
   * 3 -- edge triggered
   * 4 -- initialization command
   */
   
  out8(MASTER_COMMAND_PORT, bit(4) | bit(0));
  
  out8(MASTER_DATA_PORT, IDT_FIRST_MASTER_DESCRIPTOR);
  out8(MASTER_DATA_PORT, bit(2)); // slave connected to 3rd input
  out8(MASTER_DATA_PORT, 1); // ??
  
  
  /*
   * Initialize slave controller
   */
  
  out8(SLAVE_COMMAND_PORT, bit(4) | bit(0)); // same meaning
  
  out8(SLAVE_DATA_PORT, IDT_FIRST_SLAVE_DESCRIPTOR);
  out8(SLAVE_DATA_PORT, 2); // slave connected to 3rd input
  out8(SLAVE_DATA_PORT, 1); // ??
  
  /*
   * Mask devices as they are not initialized
   */
  
  mask_devices(0xffffu);
  __asm__("sti"); // enable interruptions on CPU
  
  write_string_to_stdout("Interrupt controllers are initialized\n");
}

void end_of_interrupt_master() {
  out8(MASTER_COMMAND_PORT, bit(5));
}

void end_of_interrupt_slave() {
  out8(SLAVE_COMMAND_PORT, bit(5));
}

uint16_t masked_devices = 0;
void set_masking() {
  masked_devices &= 0xffffu ^ bit(2);
  out8(MASTER_DATA_PORT, low_byte(masked_devices));
  out8(SLAVE_DATA_PORT, high_byte(masked_devices));
}

void mask_devices(uint16_t devices) {
  masked_devices |= devices;
  set_masking();
}

void unmask_devices(uint16_t devices) {
  masked_devices &= masked_devices ^ devices;
  set_masking();
}


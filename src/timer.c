#include <timer.h>
#include <utils.h>
#include <ioport.h>
#include <int_controller.h>

#define TIMER_DEVICE_INPUT 0
#define TIMER_COMMAND_PORT 0x43
#define ZERO_CHANNEL_PORT 0x40

void init_timer(uint8_t timer_mode, uint16_t counter_value) {
  mask_devices(bit(TIMER_DEVICE_INPUT));
  
  /*
   * 4th and 5th bits tell that channel port needs two bytes
   * for counter initial value
   */
  out8(TIMER_COMMAND_PORT, (timer_mode << 1) | bit(4) | bit(5));
  out8(ZERO_CHANNEL_PORT, low_byte(counter_value));
  out8(ZERO_CHANNEL_PORT, high_byte(counter_value));
  
  unmask_devices(bit(TIMER_DEVICE_INPUT));
}


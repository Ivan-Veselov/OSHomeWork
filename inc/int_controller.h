#ifndef __INT_CONTROLLER_H__
#define __INT_CONTROLLER_H__

#include <stdint.h>

#define MASTER_COMMAND_PORT 0x20
#define MASTER_DATA_PORT 0x21

#define SLAVE_COMMAND_PORT 0xA0
#define SLAVE_DATA_PORT 0xA1

#define IDT_FIRST_MASTER_DESCRIPTOR 32
#define IDT_LAST_MASTER_DESCRIPTOR 39
#define IDT_SLAVE_DESCRIPTOR 34

#define IDT_FIRST_SLAVE_DESCRIPTOR 40
#define IDT_LAST_SLAVE_DESCRIPTOR 47

void init_int_controller();
void end_of_interrupt_master();
void end_of_interrupt_slave();

void mask_devices(uint16_t devices);
void unmask_devices(uint16_t devices);

#endif /*__INT_CONTROLLER_H__*/

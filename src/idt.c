#include <idt.h>
#include <desc.h>
#include <serial.h>
#include <stdint.h>
#include <utils.h>

#define DESCRIPTORS_NUM 1

extern uint64_t interruption_handlers_tbl[];

struct idt_descriptor {
  uint32_t padding;
  uint32_t offset_high;
  uint16_t offset_mid;
  uint16_t flags;
  uint16_t seg_selector;
  uint16_t offset_low;
} __attribute__((packed));

struct idt_descriptor idt[DESCRIPTORS_NUM];

void write_descriptor(uint8_t index,
                      uint64_t offset,
                      uint16_t seg_selector,
                      uint8_t dpl, // 2 bits
                      uint8_t type) // 4 bits
{
  struct idt_descriptor *desc = idt + index;
  
  desc->padding = 0;
  
  desc->offset_high = high_dword(offset);
  desc->offset_mid = high_word(low_dword(offset));
  desc->offset_low = low_word(low_dword(offset));
  
  desc->seg_selector = seg_selector;
  
  // 15th bit is a validation bit
  // IST (first ? bits) is set to zero
  desc->flags = bit(15) | (dpl << 13) | (type << 8);
}

void init_idt() {
  /*
   * All descriptors' validation bit is 0.
   * Therefore all of them are invalid right now.
   */
  
  /*
   * Write descriptors to IDT
   */

  write_descriptor(0, interruption_handlers_tbl[0], 0, 3, 14);
  
  /*
   * Write IDT size (in bytes) and adress to idt register
   */
  
  struct desc_table_ptr ptr = {sizeof(idt) - 1, (uint64_t)idt};
  write_idtr(&ptr);
  
  // write_string_to_stdout("...\n");
}


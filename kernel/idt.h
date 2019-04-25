#ifndef _IDT_H_
#define _IDT_H_
#include "types.h"

// The offset is split in 2 parts. It represents the address
// of the entry point of the ISR (Interrupt Service Routine).
// Selector must point to a valid descriptor in the GDT.
// Type_attr is structured as follows:

//   7                           0
// +---+---+---+---+---+---+---+---+
// | P |  DPL  | S |    GateType   |
// +---+---+---+---+---+---+---+---+

// P - present (set to 0 for unused interrupts)
// DPL - descriptor privilege level (specifies which minimum privilege
// level the calling descriptor should have)
// S - storage segment (set to 0 for interrupt and trap gates)
// Gate type - gate type. 5 - 32b task gate; 6 - 16b interrupt gate;
// 7 - 16b trap gate; 14 - 32b interrupt gate; 15 - 32b trap gate.

struct idt_descr {
    uint16_t offset_1;  // bits 0-15, defines the offset
    uint16_t selector;  // code segment selector in GDT
    uint8_t ist;    // bits 0-2 hold interrupt stack table offset, the rest are 0
    uint8_t type_attr;  // type and attributes
    uint16_t offset_2;  // bits 16-31, offset
    uint32_t offset_3;  // bits 32-63, offset
    uint32_t zero;  //reserved
} __attribute__((packed));

struct idt_ptr {
    uint16_t limit;
    uint64_t base;
} __attribute__((packed));

struct idt_descr idt[256];
struct idt_ptr idtp;    // pointer used in assembly

void idt_install();
void idt_set_gate(uint8_t num, uint64_t base, uint16_t sel, uint8_t type);

#endif
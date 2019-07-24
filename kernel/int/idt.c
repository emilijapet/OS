#include "../types.h"
#include "idt.h"
#include "isrs.h"

// function defined in boot.S
extern void idt_load();

void idt_install(){
    idtp.limit = (sizeof(struct idt_descr) * 256) - 1;
    idtp.base = (uint64_t) &idt;

    // initialize the idt to 0
    memset((uint8_t*) &idt, 0, sizeof(struct idt_descr) * 256);

    // Install the 32 gates in the IDT
    idt_set_gate(0, (uint64_t) &_isr0, 0x08, 0x8E);
    idt_set_gate(1, (uint64_t) &_isr1, 0x08, 0x8E);
    idt_set_gate(2, (uint64_t) &_isr2, 0x08, 0x8E);
    idt_set_gate(3, (uint64_t) &_isr3, 0x08, 0x8E);
    idt_set_gate(4, (uint64_t) &_isr4, 0x08, 0x8E);
    idt_set_gate(5, (uint64_t) &_isr5, 0x08, 0x8E);
    idt_set_gate(6, (uint64_t) &_isr6, 0x08, 0x8E);
    idt_set_gate(7, (uint64_t) &_isr7, 0x08, 0x8E);
    idt_set_gate(8, (uint64_t) &_isr8, 0x08, 0x8E);
    idt_set_gate(9, (uint64_t) &_isr9, 0x08, 0x8E);
    idt_set_gate(10, (uint64_t) &_isr10, 0x08, 0x8E);
    idt_set_gate(11, (uint64_t) &_isr11, 0x08, 0x8E);
    idt_set_gate(12, (uint64_t) &_isr12, 0x08, 0x8E);
    idt_set_gate(13, (uint64_t) &_isr13, 0x08, 0x8E);
    idt_set_gate(14, (uint64_t) &_isr14, 0x08, 0x8E);
    idt_set_gate(15, (uint64_t) &_isr15, 0x08, 0x8E);
    idt_set_gate(16, (uint64_t) &_isr16, 0x08, 0x8E);
    idt_set_gate(17, (uint64_t) &_isr17, 0x08, 0x8E);
    idt_set_gate(18, (uint64_t) &_isr18, 0x08, 0x8E);
    idt_set_gate(19, (uint64_t) &_isr19, 0x08, 0x8E);
    idt_set_gate(20, (uint64_t) &_isr20, 0x08, 0x8E);
    idt_set_gate(21, (uint64_t) &_isr21, 0x08, 0x8E);
    idt_set_gate(22, (uint64_t) &_isr22, 0x08, 0x8E);
    idt_set_gate(23, (uint64_t) &_isr23, 0x08, 0x8E);
    idt_set_gate(24, (uint64_t) &_isr24, 0x08, 0x8E);
    idt_set_gate(25, (uint64_t) &_isr25, 0x08, 0x8E);
    idt_set_gate(26, (uint64_t) &_isr26, 0x08, 0x8E);
    idt_set_gate(27, (uint64_t) &_isr27, 0x08, 0x8E);
    idt_set_gate(28, (uint64_t) &_isr28, 0x08, 0x8E);
    idt_set_gate(29, (uint64_t) &_isr29, 0x08, 0x8E);
    idt_set_gate(30, (uint64_t) &_isr30, 0x08, 0x8E);
    idt_set_gate(31, (uint64_t) &_isr31, 0x08, 0x8E);    

    idt_load();
}

void idt_set_gate(uint8_t num, uint64_t base, uint16_t sel, uint8_t type){
    // initialize the offset
    idt[num].offset_1 = (base & 0xFFFF);
    idt[num].offset_2 = (base >> 16) & 0xFFFF;
    idt[num].offset_3 = (base >> 32) & 0xFFFFFFFF;

    idt[num].selector = sel;
    idt[num].type_attr = type;

    idt[num].ist = 0;
    idt[num].zero = 0;
}
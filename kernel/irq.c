#include "sys.h"

void *irq_routines[16] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

void install_irq_handler(int irq, void (*handler)(struct interrupt_stack *r)){
    irq_routines[irq] = handler;
}

void uninstall_irq_handler(int irq){
    irq_routines[irq] = 0;
}

void irq_remap(void){
    outportb(0x20, 0x11);
    outportb(0xA0, 0x11);
    outportb(0x21, 0x20);
    outportb(0xA1, 0x28);
    outportb(0x21, 0x04);
    outportb(0xA1, 0x02);
    outportb(0x21, 0x01);
    outportb(0xA1, 0x01);
    outportb(0x21, 0x0);
    outportb(0xA1, 0x0);
}

void irq_install(){
    irq_remap();

    idt_set_gate(32, (uint64_t) &_irq0, 0x08, 0x8E);
    idt_set_gate(33, (uint64_t) &_irq1, 0x08, 0x8E);
    idt_set_gate(34, (uint64_t) &_irq2, 0x08, 0x8E);
    idt_set_gate(35, (uint64_t) &_irq3, 0x08, 0x8E);
    idt_set_gate(36, (uint64_t) &_irq4, 0x08, 0x8E);
    idt_set_gate(37, (uint64_t) &_irq5, 0x08, 0x8E);
    idt_set_gate(38, (uint64_t) &_irq6, 0x08, 0x8E);
    idt_set_gate(39, (uint64_t) &_irq7, 0x08, 0x8E);
    idt_set_gate(40, (uint64_t) &_irq8, 0x08, 0x8E);
    idt_set_gate(41, (uint64_t) &_irq9, 0x08, 0x8E);
    idt_set_gate(42, (uint64_t) &_irq10, 0x08, 0x8E);
    idt_set_gate(43, (uint64_t) &_irq11, 0x08, 0x8E);
    idt_set_gate(44, (uint64_t) &_irq12, 0x08, 0x8E);
    idt_set_gate(45, (uint64_t) &_irq13, 0x08, 0x8E);
    idt_set_gate(46, (uint64_t) &_irq14, 0x08, 0x8E);
    idt_set_gate(47, (uint64_t) &_irq15, 0x08, 0x8E);
}

void irq_handler(struct interrupt_stack *r){
    void (*handler)(struct interrut_stack *r);

    handler = irq_routines[r->int_no - 32];
    if(handler){
        handler(r);
    }

    if(r->int_no >= 40){
        outportb(0xA0, 0x20);
    }

    outportb(0x20, 0x20);
}
#ifndef _ISR_H_
#define _ISR_H_

// struct interrupt_stack {
//     unsigned long int gs, fs;    // Registers we pushed manually
//     unsigned long int rsi, rdi, rax, rcx, rdx, r8, r9, r10, r11;    // Registers pushed by PUSHA
//     unsigned int int_no, err_code;  // The interrupt code and error code we pushed
//     unsigned long int eip, cs, eflags, useresp, ss;  // Pushed by the processor automatically
// };

struct interrupt_stack {
    uint64_t ds;
    uint64_t rdi, rsi, rbp, rsp, rbx, rdx, rcx, rax;
    uint64_t int_no, err_code;
    uint64_t rip, cs, eflags, useresp, ss;
};

void fault_handler(struct interrupt_stack *r);

// ISR prototypes from boot.S
extern void _isr0();
extern void _isr1();
extern void _isr2();
extern void _isr3();
extern void _isr4();
extern void _isr5();
extern void _isr6();
extern void _isr7();
extern void _isr8();
extern void _isr9();
extern void _isr10();
extern void _isr11();
extern void _isr12();
extern void _isr13();
extern void _isr14();
extern void _isr15();
extern void _isr16();
extern void _isr17();
extern void _isr18();
extern void _isr19();
extern void _isr20();
extern void _isr21();
extern void _isr22();
extern void _isr23();
extern void _isr24();
extern void _isr25();
extern void _isr26();
extern void _isr27();
extern void _isr28();
extern void _isr29();
extern void _isr30();
extern void _isr31();

#endif
#ifndef _ISR_H_
#define _ISR_H_

void fault_handler(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9, uint64_t rax, uint64_t r10, uint64_t r11, uint64_t int_no, uint64_t err_code, uint64_t eip, uint64_t cs, uint64_t eflags, uint64_t useresp, uint64_t ss);

// struct interrupt_stack {
//     uint64_t ds;
//     uint64_t rdi, rsi, rbp, rsp, rbx, rdx, rcx, rax;
//     uint64_t int_no, err_code;
//     uint64_t rip, cs, eflags, useresp, ss;
// };

// void fault_handler(struct interrupt_stack *r);

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
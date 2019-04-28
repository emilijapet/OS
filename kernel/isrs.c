#include "sys.h"

// Just an array containing the interrupt messages
char *exception_messages[] = {
    "Division by 0",
    "Debug",
    "Non-maskable interrupt",
    "Breakpoint",
    "Into detected overflow",
    "Out of bounds",
    "Invalid opcode",
    "No coprocessor found",
    "Double fault",
    "Coprocessor segment overrun",
    "Bad TSS",
    "Segment not present",
    "Stack fault",
    "General protection fault",
    "Page fault",
    "Unknown interrupt",
    "Coprocessor fault",
    "Alignment check",
    "Machine check",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved"
};

void fault_handler(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9, uint64_t rax, uint64_t r10, uint64_t r11, uint64_t int_no, uint64_t err_code, uint64_t eip, uint64_t cs, uint64_t eflags, uint64_t useresp, uint64_t ss){
    // Handle the known faults

    if(int_no < 32){
        puts((uint8_t*)exception_messages[int_no]);
        puts((uint8_t*)" exception. System halted.\n");
        for(;;);
    } else {
        printf("Uknown interrupt ir %d; ec %d! Continue execution.\n", int_no, err_code);
        // for(;;);
    }
}

/*
    save the arguments (error code and interrupt number) in registers - rdi and rsi
    push some registers on the stack:
        rax
        rcx
        rdx
        rsi
        rdi
        r8
        r9
        r10
        r11
*/
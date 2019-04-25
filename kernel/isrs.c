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

void fault_handler(struct interrupt_stack *r){
    // Handle the known faults

    if((r->int_no) < 32){
        puts((uint8_t*)exception_messages[r->int_no]);
        puts((uint8_t*)" exception. System halted.\n");
        for(;;);
    } else {
        printf("Uknown interrupt ir %d; ec %d! Continue execution.\n", r->int_no, r->err_code);
        // for(;;);
    }
}
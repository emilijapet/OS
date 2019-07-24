#include "../types.h"

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

void page_fault_handler(uint64_t error_code){
    // The faulting address is stored in the CR2 register
    uint64_t faulting_address;
    asm volatile("movq %%cr2, %0" : "=r" (faulting_address));

    int present = !(error_code & 0x1);
    int write = error_code & 0x2;
    int user = error_code & 0x4;
    int reserved = error_code & 0x8;
    int instruction = error_code & 0x10;

    printf("Page fault!\nPresent: %d\nWrite: %d\nUser: : %d\nReserved: %d\nInstruction: %d\n",
        present, write, user, reserved, instruction);
    
    // Halt execution
    for(;;);
}

void fault_handler(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9, uint64_t rax, uint64_t r10, uint64_t r11, uint64_t int_no, uint64_t err_code, uint64_t eip, uint64_t cs, uint64_t eflags, uint64_t useresp, uint64_t ss){
    // Handle the known faults

    if(int_no == 14){
        page_fault_handler(err_code);
    }

    if(int_no < 32){
        puts((uint8_t*)exception_messages[int_no]);
        puts((uint8_t*)" exception. System halted.\n");
        for(;;);
    } else {
        printf("Uknown interrupt ir %d; ec %d! Continue execution.\n", int_no, err_code);
        // for(;;);
    }
}
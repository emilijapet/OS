#include "sys.h"

void Kernel_Main(void){
    vga_init();
    puts((uint8_t*)"Initialize IDT\n");

    idt_install();
    // irq_install();
    // __asm__ __volatile__ ("sti");

    // _isr0();
    // _isr8();
    _isr10();

    // int i = 2;
    // putchar(i / 0);
    puts((uint8_t*)"Test this bitch");
    while(1);
}
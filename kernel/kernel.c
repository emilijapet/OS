#include "idt.h"
#include "scrn.h"
#include "types.h"

void Kernel_Main(void){
    vga_init();
    puts((uint8_t*)"Initialize IDT\n");
    idt_install();

    puts((uint8_t*)"Test this bitch");
    while(1);
}
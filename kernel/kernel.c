#include "sys.h"
#include "multiboot2.h"

void Kernel_Main(multiboot_info_t *mbi){
    vga_init();
    puts((uint8_t*)"Initialize IDT\n");

    // multiboot_memory_map_t* mmm2 = (multiboot_memory_map_t*) ((uint64_t) mmm->addr + mmm->len );
    // printf("MMM2 length: %d", mmm2->len);

    // printf("The magic number is %d", magic);

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
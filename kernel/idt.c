#include "idt.h"
#include "types.h"

// function defined in boot.S
extern void idt_load();

void *memset_c(void *s, int c, int n){
    unsigned char* p = s;
    while(n--){
        *p++ = (unsigned char) c;
    }
    return s;
}

void idt_install(){
    idtp.limit = (sizeof(struct idt_descr) * 256) - 1;
    idtp.base = (unsigned long int) &idt;

    // initialize the idt to 0
    memset_c(&idt, 0, sizeof(struct idt_descr) * 256);

    idt_load();
}

void idt_set_gate(int16_t num, uint32_t base, uint16_t sel, uint8_t flags){
    // initialize the offset
    idt[num].offset_1 = (base & 0xFFFF);
    idt[num].offset_2 = (base >> 16) & 0xFFFF;
    idt[num].offset_3 = (base >> 16) & 0xFFFFFFFF;

    idt[num].selector = sel;
    idt[num].ist = 0x0;   // figure this out
    idt[num].type_attr = flags;
    idt[num].zero = 0x0;
}
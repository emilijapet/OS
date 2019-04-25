#include "sys.h"

uint8_t inbyte(uint16_t port){
    uint8_t ret;
    __asm__ __volatile__ ("inb %1, %0" : "=a" (ret) : "d" (port));
    return ret;
}

void outbyte(uint16_t port, uint8_t data){
    __asm__ __volatile__ ("outb %1, %0" : : "d" (port), "a" (data));
}
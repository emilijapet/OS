#include "sys.h
#include "kheap.h"

uint64_t kmalloc(uint64_t size, int align, uint64_t *physical_addr){
    // If the address isn't page aligned yet, do that
    if(align == 1 && placement_addr & 0xFFFFF000){
        placement_addr &= 0xFFFFF000;
        placement_addr += 0x1000;
    }

    // If we're given a physical address, save the placement in it
    if(physical_addr){
        *physical_addr = placement_addr;
    }

    uint64_t tmp = placement_addr;
    placement_addr += size;
    return tmp;
}
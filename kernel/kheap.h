#ifndef _KHEAP_H_
#define _KHEAP_H_

#include "types.h"

uint64_t placement_addr;
uint64_t kmalloc(uint64_t size, int align, uint64_t *physical_addr);

#endif
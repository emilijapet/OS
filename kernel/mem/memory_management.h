#ifndef _MMAN_H_
#define _MMAN_H_

#include "types.h"
uint64_t kmalloc(size_t size);
uint64_t kfree(uint64_t start_address);

#endif
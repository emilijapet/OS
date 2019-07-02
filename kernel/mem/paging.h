#ifndef _PAGING_H_
#define _PAGING_H_

#include "types.h"

uint64_t alloc_pages(size_t n);
int free_pages(uint64_t start_address);
void initialize_paging(uint64_t memory_size);

#endif
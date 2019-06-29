#ifndef _PAGE_M_
#define _PAGE_M_

#define PAGE_SIZE 0x1000

uint64_t create_list_entry(uint64_t address, uint64_t size);
uint64_t remove_list_entry(uint64_t address);
uint64_t allocate_pages(uint64_t n);

#endif
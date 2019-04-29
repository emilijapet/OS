#ifndef _PAGING_H_
#define _PAGING_H_

#define NUMBER_OF_PAGES 1024;
#include "sys.h"

typedef struct page {
    uint64_t present    : 1;
    uint64_t read_write : 1;    // Read only if it's clear, read write of not
    uint64_t user       : 1;    // Supervisor level (?) only if it's clear
    uint64_t accessed   : 1;    // Marks if the page had been accessed
    uint64_t dirty      : 1;    // Marks if the page had been edited
    uint64_t unused     : 7;    // Union of unused or reserved bits
    uint64_t frame      : 20;   // Frame address
} page_t;

typedef struct page_table {
    page_t pages[NUMBER_OF_PAGES];
} page_table_t;

typedef struct page_directory {
    // Array of pointers to page tables
    page_table_t *tables[NUMBER_OF_PAGES];

    // Pointers to physical addresses of the same page tables
    // (for loading into CR3)
    uint64_t tables_physical[NUMBER_OF_PAGES];

    // The physical address of tables_physical
    uint64_t physical_addr;
} page_directory_t;

void initialize_paging();
void switch_page_directory(page_directory_t *directory);
page_t *get_page(uint64_t address, int make, page_directory_t *directory);
// void page_fault(registers_t regs);

#endif
#ifndef _ALLOC_H_
#define _ALLOC_H_

#include "types.h"

#define PAGE_SIZE 4096

typedef struct page {
    uint64_t present    : 1;
    uint64_t read_write : 1;    // Read only if it's clear, read write of not
    uint64_t user       : 1;    // If it's not set, only kernel mode code can access it
    uint64_t write_thorugh    : 1;    // Writes go directly to memory if set
    uint64_t caching    : 1;    // Don't cache the page if this is set
    uint64_t accessed   : 1;    // Marks if the page had been accessed
    uint64_t dirty      : 1;    // Marks if the page had been edited
    uint64_t huge_page  : 1;    // Must be 0 in level P1 and P4, creates a 1GB page in P3 and a 2MB page in P2
    uint64_t global     : 1;    // Page isn't flushed from cache on address space switch (PGE bit in CR4 must be set for this)
    uint64_t available_1  : 3;    // Can be used freely by the OS
    uint64_t physical_addr  : 40;   // The page aligned 52 bit physical address of the frame or the next page table
    uint64_t available_2    : 11;   // Can be used freely by the OS
    uint64_t no_execute : 1;    // Forbid executing code on this page
} page_t;

typedef struct page_table {
    page_t entries[512];
    uint64_t last_entry;
} page_table_t;

page_table_t pt_c, pdt_c, pdpt_c, pml4_c;
uint64_t map_memory(unsigned long addr);

#endif
#ifndef _KHEAP_H_
#define _KHEAP_H_

#define HEAP_MAGIC        0x123890AB
#include "types.h"
#include "ordered_list.h"

typedef struct header {
    uint64_t magic;
    uint8_t free;
    uint64_t size;
} header_t;

typedef struct footer {
    uint64_t magic;
    header_t *header;
} footer_t;

typedef struct heap {
    ordered_list_t index;
    uint64_t start_address;
    uint64_t end_address;
    uint64_t max_address;
    uint8_t supervisor;
    uint8_t read_only;
} heap_t;

heap_t *create_heap(uint64_t start, uint64_t end, uint64_t max, uint8_t supervisor, uint8_t read_only);
void *alloc(uint64_t size, uint8_t page_align, heap_t *heap);
void free(void *p, heap_t *heap);
uint64_t kmalloc_internal(uint64_t size, int align, uint64_t *physical_location);
uint64_t kmalloc_aligned(uint64_t size);
uint64_t kmalloc_physical(uint64_t size, uint64_t *physical_location);
uint64_t kmalloc_aligned_physical(uint64_t size, uint64_t *physical_location);
uint64_t kmalloc(uint64_t size);
void kfree(uint64_t *p);

#endif
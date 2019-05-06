#ifndef _LINKEDMEM_H_
#define _LINKEDMEM_H_

#include "multiboot2.h"
#include "types.h"

typedef struct frame {
    uint64_t addr;
    uint64_t len;
    struct frame *next_frame;
} frame_t;

frame_t *head;
frame_t *current;

void append_frame(multiboot_memory_map_t *mmap);
int size();

#endif
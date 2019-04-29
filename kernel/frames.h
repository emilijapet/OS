#ifndef _FRAMES_H_
#define _FRAMES_H_

#include "types.h"
#include "paging.h"

uint64_t *frames;
uint64_t frame_count;

#define INDEX_FROM_BIT(a) (a/64)
#define OFFSET_FROM_BIT(a) (a%64)

static void set_frame(uint64_t frame_addr);
static void clear_frame(uint64_t frame_addr);
static uint64_t test_frame(uint64_t frame_addr);
static uint64_t first_frame();
void alloc_frame(page_t *page, int is_kernel, int is_writeable);
void free_frame(page_t *page);

#endif
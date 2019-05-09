#ifndef _FRAMES_H_
#define _FRAMES_H_

#include "types.h"

#define INT_SIZE 64
#define BITMASK(b) (1 << ((b) % INT_SIZE))  // Returns the value of the b bit
#define BITSLOT(b) ((b) / INT_SIZE) // Returns the slot of the b bit
#define BITSET(a, b) ((a)[BITSLOT(b)] |= BITMASK(b))    // a is the bitmap itself
#define BITCLEAR(a, b) ((a)[BITSLOT(b)] &= ~BITMASK(b))
#define BITTEST(a, b) ((a)[BITSLOT(b)] & BITMASK(b))
#define BITNSLOTS(nb) ((nb + INT_SIZE - 1) / INT_SIZE)  // Defines a bitset of nb slots

uint64_t *frames;
uint64_t frame_count;

void alloc_frame(page_t *page, int is_kernel, int writeable);
void free_frame(page_t *page);

#endif
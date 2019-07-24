#ifndef _FRAMES_H_
#define _FRAMES_H_

#include "types.h"

#define CHAR_BIT 8
#define BITMASK(b) (1 << ((b) % CHAR_BIT))  // Returns the value of the b bit
#define BITSLOT(b) ((b) / CHAR_BIT) // Returns the slot of the b bit
#define BITSET(a, b) ((a)[BITSLOT(b)] |= BITMASK(b))    // a is the bitmap itself
#define BITCLEAR(a, b) ((a)[BITSLOT(b)] &= ~BITMASK(b))
#define BITTEST(a, b) ((a)[BITSLOT(b)] & BITMASK(b))
#define BITNSLOTS(nb) ((nb + CHAR_BIT - 1) / CHAR_BIT)  // Defines a bitset of nb slots

char *frames;
uint64_t frame_count;

#endif
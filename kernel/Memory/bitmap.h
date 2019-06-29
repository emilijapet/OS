#ifndef _BITMAP_H_
#define _BITMAP_H_

#define INT_SIZE 64
#define BITMASK(b) (1 << ((b) % INT_SIZE))  // Returns the value of the b bit
#define BITSLOT(b) ((b) / INT_SIZE) // Returns the slot of the b bit
#define BITSET(a, b) ((a)[BITSLOT(b)] |= BITMASK(b))    // a is the bitmap itself
#define BITCLEAR(a, b) ((a)[BITSLOT(b)] &= ~BITMASK(b))
#define BITTEST(a, b) ((a)[BITSLOT(b)] & BITMASK(b))
#define BITNSLOTS(nb) ((nb + INT_SIZE - 1) / INT_SIZE)  // Defines a bitset of nb slots

#endif
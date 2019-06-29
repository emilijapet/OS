#ifndef _FRAME_M_
#define _FRAME_M_

#define BITMAP_SIZE 1024    //TODO fix this
#define FRAME_SIZE 0x1000   //TODO fix this

uint64_t find_free_frames(uint64_t n);
uint64_t free_frames(uint64_t address, uint64_t n);

#endif
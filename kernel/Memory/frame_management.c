#include "bitmap.h"
#include "../types.h"
#include "frame_management.h"

uint64_t *frame_bitmap;

uint64_t find_free_frames(uint64_t n){
    uint64_t i, j, search_failed = 0;

    for(i = 0; i < BITMAP_SIZE; i++){
        search_failed = 0;

        if(BITTEST(frame_bitmap, i / FRAME_SIZE)){
            // we found one free bit, let's see if we can find the rest
            for(j = i + 1; j < i + n; j++){
                // if we find one frame that isn't available, get out of this loop
                if(!BITTEST(frame_bitmap, j / FRAME_SIZE)){
                    search_failed = 1;
                    break;
                }
            }

            // if we found all the frames we needed, return the address now
            if(!search_failed){
                return i * FRAME_SIZE;
            }

            // if not, we can skip all the frames we tested up until now and continue
            i = j;
        }
    }

    // return with error if we didn't find anything
    return 0;
}

uint64_t free_frames(uint64_t address, uint64_t n){
    uint64_t i;

    for(i = address / FRAME_SIZE; i < address / FRAME_SIZE + n; i++){
        BITCLEAR(frame_bitmap, i);
    }

    return 1;
}
#include "types.h"
#include "paging.h"
#include "frames.h"

// return whether or not the address is frame aligned
int check_aligned(uint64_t address){
    return !(address % 0x1000);
}

int set_frames(uint64_t start_address, uint64_t num){
    if(!check_aligned(start_address)){
        return -1;
    }

    int i;
    for(i = 0; i < num; i++){
        // if the frame is already allocated, fuck this shit
        if(BITTEST(frames, i)){
            return 0;
        }
        BITSET(frames, i);
    }

    return 1;
}

int clear_frames(uint64_t start_address, uint64_t num){
    if(!check_aligned(start_address)){
        return -1;
    }

    int i;
    for(i = 0; i < num; i++){
        // if the frame is already deallocated, fuck this shit
        if(!BITTEST(frames, i)){
            return 0;
        }
        BITSET(frames, i);
    }

    return 1;
}

uint64_t find_free_frames(size_t num){
    int i = 0;
    // Loop through the bitmap
    // Whithin that loop, keep looping to find n consecutive free frames

    // Go through every frame in memory
    while(i < frame_count){
        if(BITTEST(frames, i)){
            // If the frame is free, create a variable to count how many frames
            // we've already found
            int j = 1;
            while(j < num){
                // Test the next frame. If it's also free, increase the counter of
                // found free frames. If the frame is not free, we won't find n consecutive free
                // frames from this point, so break this loop
                i++;
                if(BITTEST(frames, i)){
                    j++;
                } else {
                    break;
                }
            }

            // If we found the n consecutive free frames, we don't need to loop anymore, return the address of the first frame.
            if(j == num){
                return (i - j) * 0x1000;
            }
        }
        i++;
    }
    return 0;
}
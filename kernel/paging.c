#include "paging.h"
#include "frames.h"

void initialize_paging(uint64_t memory_size){
    frame_count = memory_size / 0x4000;
    frames = BITNSLOTS(frame_count);
    memset(frames, 0, frame_count);
}
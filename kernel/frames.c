#include "sys.h"

extern uint64_t placement_addr;

static void set_frame(uint64_t frame_addr){
    uint64_t frame = frame_addr/0x1000;
    uint64_t index = INDEX_FROM_BIT(frame);
    uint64_t offset = OFFSET_FROM_BIT(frame);
    frames[index] |= (0x1 << offset);
}

static void clear_frame(uint64_t frame_addr){
    uint64_t frame = frame_addr/0x1000;
    uint64_t index = INDEX_FROM_BIT(frame);
    uint64_t offset = OFFSET_FROM_BIT(frame);
    frames[index] &= ~(0x1 << offset);
}

static uint64_t test_frame(uint64_t frame_addr){
    uint64_t frame = frame_addr/0x1000;
    uint64_t index = INDEX_FROM_BIT(frame);
    uint64_t offset = OFFSET_FROM_BIT(frame);
    return (frames[index] & 0x1 << offset);
}

static uint64_t first_frame(){
    uint64_t i, j;
    for(i = 0; i < INDEX_FROM_BIT(frame_count); i++){
        if(frames[i] != 0xFFFFFFFFFFFFFFFF){
            for(j = 0; j < 64; j++){
                uint64_t to_test = 0x1 << j;
                if(!frames[i] & to_test){
                    return i * 64 + j;
                }
            }
        }
    }
}

void alloc_frame(page_t *page, int is_kernel, int is_writeable){
    if(page->frame != 0){
        return; // Frame was already allocated
    } else {
        uint64_t index = first_frame();
        
        if(index == (uint64_t) - 1){
            printf("No free frames!");
            for(;;);
        }

        set_frame(index * 0x1000);
        page->present = 1;
        page->read_write = is_writeable ? 1 : 0;
        page->user = is_kernel ? 0 : 1;
        page->frame = index;
    }
}

void free_frame(page_t *page){
    uint64_t frame;
    if(frame != page->frame){
        return; // The given page didn't have an allocated frame
    } else {
        clear_frame(frame);
        page->frame = 0;
    }
}
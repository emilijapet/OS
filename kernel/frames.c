#include "types.h"
#include "paging.h"

uint64_t *frames;
uint64_t frame_count;
#define INDEX_FROM_BIT(a) (a/(64))
#define OFFSET_FROM_BIT(a) (a%(64))

#define INT_SIZE 64
#define BITMASK(b) (1 << ((b) % INT_SIZE))  // Returns the value of the b bit
#define BITSLOT(b) ((b) / INT_SIZE) // Returns the slot of the b bit
#define BITSET(a, b) ((a)[BITSLOT(b)] |= BITMASK(b))    // a is the bitmap itself
#define BITCLEAR(a, b) ((a)[BITSLOT(b)] &= ~BITMASK(b))
#define BITTEST(a, b) ((a)[BITSLOT(b)] & BITMASK(b))
#define BITNSLOTS(nb) ((nb + INT_SIZE - 1) / INT_SIZE)  // Defines a bitset of nb slots

static void set_frame(uint64_t frame_address){
    // Divide the address by page size? It's 4kb, right? Then this
    // will give the index of the frame. So, if I'm accessing a frame
    // at address 8KB, this will return 2
    uint64_t frame = frame_address / 0x4000;
    BITSET(frames, frame);
}

static void clear_frame(uint64_t frame_address){
    uint64_t frame = frame_address / 0x4000;
    BITCLEAR(frames, frame);
}

static uint64_t test_frame(uint32_t frame_address){
    uint64_t frame = frame_address / 0x4000;
    BITTEST(frames, frame);
}

static int64_t first_frame(){
    uint64_t i, j;

    // Now loop through the bitset until the last frame
    for(i = 0; i < BITSLOT(frame_count); i++){
        // If there are no free frames, skip this iteration.
        if(frames[i] == 0xFFFFFFFFFFFFFFFF){
            continue;
        }

        // Otherwise, go though every bit and return the first free frame
        for(j = 0; j < INT_SIZE; j++){
            if(BITTEST(frames, i)){
                return i * INT_SIZE + j;
            }
        }
    }

    // If we didn't find anything, return an error number
    return -1;
}

void alloc_frame(page_t *page, int is_kernel, int writeable){
    int64_t free_frame_index = first_frame();

    if(free_frame_index < 0){
        // Halt the system
        for(;;);
    }

    set_frame(free_frame_index * 0x4000);
    page->present = 1;
    page->read_write = writeable ? 1 : 0;
    page->user = is_kernel ? 0 : 1;
    page->physical_addr = free_frame_index * 0x4000;
}

void free_frame(page_t *page){
    // If the page didn't have a frame, just exit
    if(!page->physical_addr){
        return;
    }

    // Otherwise, clear the bit for it in the bitmap and empty the address field on the page
    clear_frame(page->physical_addr);
    page->physical_addr = 0x0;
}
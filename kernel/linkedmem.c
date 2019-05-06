#include "linkedmem.h"

frame_t *head;
frame_t *current;

void append_frame(multiboot_memory_map_t *mmap){
    frame_t *new_frame = malloc(sizeof(frame_t));
    new_frame->addr = mmap->addr;
    new_frame->len = mmap->len;
    new_frame->next_frame = 0x0;

    if(!head){
        head = new_frame;
        current = new_frame;
    } else {
        current->next_frame = new_frame;
        current = new_frame;
    }
}

int size(){
    int result = 1;
    frame_t *cur = head;

    while(cur->next_frame != 0){
        result++;
    }
    return result;
}
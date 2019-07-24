#include "paging.h"
#include "frames.h"
#include "linked_list.h"

extern uint64_t _kernel_physical_start, _kernel_physical_end, _kernel_virtual_start, _kernel_virtual_end;
uint64_t placement_address = (uint64_t) &_kernel_physical_end;

uint64_t kmalloc_c(uint64_t size, int align, uint64_t *physical_address){
    // Align the placement address;
    if(align == 1 && placement_address & 0xFFFFFFFFFF000000){
        placement_address &= 0xFFFFFFFFFF000000;
        placement_address += 0x1000;
    }

    // Save the physical address
    if(physical_address){
        *physical_address = placement_address;
    }

    uint64_t temp = placement_address;
    placement_address += size;
    return temp;
}

int create_list_entry(uint64_t address, size_t size){
    // if there are no entries yet, create the first one
    if(!head){
        head = kmalloc_c(sizeof(node_t), 0, 0);
        memset(head, 0, sizeof(node_t));
        head->starting_address = address;
        head->size = size;
        return 1;
    }

    // otherwise, continue with the regular process
    node_t *n = kmalloc_c(sizeof(node_t), 0, 0);    // TODO: fix this;
    memset(n, 0, sizeof(node_t));
    n->starting_address = address;
    n->size = size;
    n->next_node = 0;

    // go through the list to find the last entry
    node_t *cur_node = head;
    while(cur_node->next_node){
        cur_node = cur_node->next_node;
    }

    // n->previous_node = cur_node;
    cur_node->next_node = n;

    return 1;
}

node_t * get_list_entry(uint64_t address){
    // if there's no first node defined, return an error
    if(!head){
        return 0;
    }

    node_t *cur_node = head;
    do {
        if(cur_node->starting_address == address){
            return cur_node;
        }
        cur_node = cur_node->next_node;
    } while(cur_node->size);

    return 0;
}

int remove_list_entry(uint64_t address){
    // if there's no first node defined, return an error
    if(!head->size){
        return 0;
    }

    node_t *cur_node = head, *prev_node = 0;
    do {
        if(cur_node->starting_address == address){
            // if there were no nodes before this, replace the head with the next node.
            // otherwise, if it's not the last node in the list, create a new link from prev to next node
            if(!prev_node && cur_node->next_node){
                head = cur_node->next_node;
            } else if(cur_node->next_node){
                prev_node->next_node = cur_node->next_node;
            }
            return 1;
        }
        prev_node = cur_node;
        cur_node = cur_node->next_node;
    } while(cur_node->size);

    return 0;
}

uint64_t alloc_pages(size_t n){
    // Find n consecutive free frames
    // if we couldn't find that many free frames, leave now
    uint64_t start_address = find_free_frames(n);
    if(!start_address){
        return 0;
    }

    // set the frames as allocated.
    // if there was an issue setting the frames, return now
    int64_t success = set_frames(start_address, n);
    if(!success){
        return 0;
    }

    // create linked list entry for this allocated memory and return the address
    create_list_entry(start_address, (int64_t)n * 0x1000);
    return start_address;
}

int free_pages(uint64_t start_address){
    // get the list entry
    node_t *node = get_list_entry(start_address);

    if(!node){
        return 0;
    }

    // free the frames
    int success = clear_frames(start_address, node->size / 0x1000);
    if(!success){
        return 0;
    }

    // remove the list entry
    success = remove_list_entry(start_address);
    return success;
}

void initialize_paging(uint64_t memory_size){
    // Initialize paging
    // First, initialize the bitmap

    // Number of frames is size of physical memory divided
    // by frame size. 0x1000 is 4KB
    frame_count = memory_size / 0x1000;

    // Place the bitset next to the kernel
    frames = (uint64_t *) kmalloc_c(sizeof(BITNSLOTS(frame_count)), 0, 0);

    // Initialize the bitset and null it
    // *frames = BITNSLOTS(frame_count);
    memset(frames, 0, frame_count);

    // Identity map all the memory
    int i;
    for(i = 0; i < memory_size; i += 0x1000){
        // Create a page table entry for this, probably call into assembly for this

        // Then, if the piece of memory is within kernel bounds, mark it as allocated
        if(i <= 0x200000 || (i >= 0x400000 && i <= 0x600000)){
            BITSET(frames, i / 0x1000);
        }
    }
}
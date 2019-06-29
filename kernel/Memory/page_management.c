#include "../types.h"
#include "linked_list.h"
#include "page_management.h"
#include "frame_management.h"

node__t first_node;

uint64_t create_list_entry(uint64_t address, uint64_t size){
    // if there are no entries yet, create the first one
    if(!first_node.size){
        first_node.starting_address = address;
        first_node.size = size;
        return 1;
    }

    // otherwise, continue with the regular process
    node__t n = kmalloc_c();    // TODO: fix this;
    n.starting_address = address;
    n.size = size;
    n.next_node = 0;

    // go through the list to find the last entry
    node__t cur_node = first_node;
    while(cur_node.next_node){
        cur_node = cur_node.next_node;
    }

    n.previous_node = cur_node;
    cur_node.next_node = n;

    return 1;
}

uint64_t remove_list_entry(uint64_t address){
    // if there's no first node defined, return an error
    if(!first_node.size){
        return 0;
    }

    node__t cur_node = first_node;

    do {
        if(cur_node.starting_address == address){
            free_frames(address, cur_node.size / FRAME_SIZE);
            
            cur_node.next_node.previous_node = cur_node.previous_node;
            cur_node.previous_node.next_node = cur_node.next_node;

            return 1;
        }
    } while(cur_node.next_node);

    return 0;
}

uint64_t allocate_pages(uint64_t n){
    uint64_t address = find_free_frames(n);
    create_list_entry(address, n * PAGE_SIZE);
    return address;
}
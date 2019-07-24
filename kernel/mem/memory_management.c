#include "types.h"
#include "paging.h"

uint64_t kmalloc(size_t size){
    return alloc_pages((uint64_t)size / 0x1000);
}

uint64_t kfree(uint64_t start_address){
    if(!start_address){
        return 0;
    }
    return free_pages(start_address);
}
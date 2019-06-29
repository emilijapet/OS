#include "../types.h"
#include "page_management.h"

uint64_t kmalloc(uint64_t size){
    uint64_t address = allocate_pages(size / PAGE_SIZE);
    return address;
}

uint64_t kfree(uint64_t address){
    uint64_t success = remove_list_entry(address);
    return success;
}
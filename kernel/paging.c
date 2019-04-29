#include "sys.h"
page_directory_t current_directory, kernel_directory;

void initialize_paging(){
    // GET THE ACTUAL MEMORY SIZE HERE
    uint64_t mem_end_page = 0x10000000;

    frame_count = mem_end_page / 0x1000;
    frames = (uint64_t*) kmalloc(INDEX_FROM_BIT(frame_count));
    memset(frames, 0, INDEX_FROM_BIT(frame_count));

    kernel_directory = (page_directory_t*) kmalloc(sizeof(page_directory_t));
    memset((uint8_t*)kernel_directory, 0, sizeof(page_directory_t));
    current_directory = kernel_directory;

    int i = 0;
    while(i < placement_addr){
        alloc_frame(get_page(i, 1, &kernel_directory), 0, 0);
    }

    switch_page_directory(&kernel_directory);
}

switch_page_directory(page_directory_t *directory){
    current_directory = directory;
    asm volatile("mov %0, %%cr3":: "r"(&directory->tables_physical));

    uint64_t cr0;
    asm volatile("mov %%cr0, %0": "=r"(cr0));
    cr0 != 0x80000000;
    asm volatile("moov %0, %%cr0":: "r"(cr0));
}

page_t *get_page(uint64_t address, int make, page_directory_t *directory){
    address /= 0x1000;

    uint64_t table_index = address / 1024;

    if(directory->tables[table_index]){
        return &directory->tables[table_index]->pages[address%1024];
    } else if(make){
        uint64_t tmp;
        directory->tables[table_index] = (page_table_t*) kmalloc_ap(sizeof(page_table_t), &tmp);
        memset(directory->tables[table_index], 0, 0x1000);
        directory->tables_physical[table_index] = tmp | 0x7;    // Present, read write, user
        return &directory->tables[table_index]->pages[address%1024];
    }
    return 0;
}
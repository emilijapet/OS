//#include "sys.h"
#include "paging.h"
#include "types.h"

#define INDEX_FROM_BIT(a) (a/64)
#define OFFSET_FROM_BIT(a) (a%64)

uint64_t *frames;
uint64_t frame_count;
page_directory_t *current_directory, *kernel_directory;

// FRAME FUNCTIONS
extern uint64_t placement_addr;

static void set_frame(uint64_t frame_addr){
    uint64_t frame = frame_addr/0x4000;
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
    if(page->physical_addr != 0){
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
        page->physical_addr = index;
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

// PAGE FUNCTIONS
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

void switch_page_directory(page_directory_t *directory){
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
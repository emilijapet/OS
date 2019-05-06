//#include "sys.h"
#include "multiboot2.h"
#include "scrn.h"
#include "idt.h"
#include "paging.h"
#include "linkedmem.h"

// TODO:
// 1) When looping through memory map, create a list of available frames.
// 2) Create frame allocation functions
// 3) Create page allocation functions
// 4) Create kmalloc function

uint64_t get_memory_size(unsigned long addr){//unsigned long magic,
    struct multiboot_tag *tag;
    uint64_t total_memory_size = 0;

    /*if(magic != MULTIBOOT2_BOOTLOADER_MAGIC){
        printf("Unaligned mbi!");
        // invalid magic number
        return 0;
    }*/

    if(addr & 7){
        printf("Unaligned mbi!");
        return 0;
    }

    for (tag = (struct multiboot_tag *) (addr + 8); tag->type != MULTIBOOT_TAG_TYPE_END; tag = (struct multiboot_tag *) ((multiboot_uint8_t *) tag + ((tag->size + 7) & ~7))){

        if(tag->type == MULTIBOOT_TAG_TYPE_MMAP){
            multiboot_memory_map_t *mmap;

            for (mmap = ((struct multiboot_tag_mmap *) tag)->entries; (multiboot_uint8_t *) mmap < (multiboot_uint8_t *) tag + tag->size; mmap = (multiboot_memory_map_t *) ((unsigned long) mmap + ((struct multiboot_tag_mmap *) tag)->entry_size)){
                if(mmap->type == MULTIBOOT_MEMORY_AVAILABLE){
                    total_memory_size += mmap->len;
                    append_frame(mmap);
                }
            }
        }
    }
    tag = (struct multiboot_tag *) ((multiboot_uint8_t *) tag + ((tag->size + 7) & ~7));
    return total_memory_size;
}

void Kernel_Main(unsigned long addr){
    vga_init();
    
    uint64_t memory_size = get_memory_size(addr);
    printf("Linked list head address: ", size());
    
    idt_install();

    uint64_t pml4_addr;
    asm volatile("movq %%cr3, %0" : "=r" (pml4_addr));
    page_t *pg = pml4_addr;
    printf("Page present: %d", pg->present);

    puts((uint8_t*)"Test this bitch");
    while(1);
}
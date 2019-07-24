#include "multiboot2.h"
#include "io/scrn.h"
#include "int/idt.h"
#include "mem/paging.h"
#include "mem/memory_management.h"
#include "io/ide.h"

extern void paging_init(uint64_t);

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
                }
            }
        }
    }
    tag = (struct multiboot_tag *) ((multiboot_uint8_t *) tag + ((tag->size + 7) & ~7));
    return total_memory_size;
}

void Kernel_Main(unsigned long multiboot_addr){
    vga_init();
    idt_install();
    
    uint64_t memory_size = get_memory_size(multiboot_addr);
    paging_init(memory_size);

    initialize_paging(memory_size);

    uint64_t s_addr = kmalloc(0x1000);
    uint64_t s_addr2 = kmalloc(0x6000);
    uint64_t s_addr3 = kmalloc(0x20000000); // 512MB


    uint64_t success2 = kfree(s_addr2);
    int success = kfree(s_addr);
    int success3 = kfree(s_addr3);

    printf("Freed %s", success);
    printf("Freed %s", success2);

    ide_init();

    puts((uint8_t*)"Test this bitch");
    while(1);
}
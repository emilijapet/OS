//#include "sys.h"
#include "multiboot2.h"
#include "scrn.h"


void get_memory_size(unsigned long addr){//unsigned long magic, 
    struct multiboot_tag *tag;
    unsigned size;

    /*if(magic != MULTIBOOT2_BOOTLOADER_MAGIC){
        // invalid magic number
        return;
    }*/

    if(addr & 7){
        //unaligned mbi
    }

    size = *(unsigned *) addr;

    for(tag = (struct multiboot_tag *) (addr + 8); tag->type != MULTIBOOT_TAG_TYPE_END; tag = (struct multiboot_tag *) ((multiboot_uint8_t *) tag + ((tag->size + 7) & ~7))){
        switch (tag->type){
            case MULTIBOOT_TAG_TYPE_MMAP:
                multiboot_memory_map_t *mmap;

                for(mmap = ((struct multiboot_tag_mmap *) tag)->entries; (multiboot_uint8_t *) mmap < (multiboot_uint8_t *) tag + tag->size; mmap = (multiboot_memory_map_t *) ((unsigned long) mmap + ((struct multiboot_tag_mmap *) tag)->entry_size)){
                    // print/save mmap entry info
                    printf((uint8_t *) "base_addr = 0x%x%x\nlength = 0x%x%x\ntype = 0x%x\n", (unsigned) (mmap->addr >> 32), (unsigned) (mmap->addr & 0xffffffff), (unsigned) (mmap->len >>32), (unsigned) (mmap->len & 0xffffffff), (unsigned) mmap->type);
                }
                break;
            default:
                // other types of tags
                break;
        }

        tag = (struct multiboot_tag *) ((multiboot_uint8_t *) tag + ((tag->size + 7) & ~7));
    }
}

void Kernel_Main(unsigned long addr){
    vga_init();
    get_memory_size(addr);
    //puts((uint8_t*)"Initialize IDT\n");

    //multiboot_memory_map_t* mmap = (multiboot_memory_map_t*) mbi->mmap_addr;

    // multiboot_memory_map_t* mmm2 = (multiboot_memory_map_t*) ((uint64_t) mmm->addr + mmm->len );
    //printf("MMM2 length: %d", mmap->len);

    // printf("The magic number is %d", magic);

    //idt_install();
    // irq_install();
    // __asm__ __volatile__ ("sti");

    // _isr0();
    // _isr8();
    //_isr10();

    // int i = 2;
    // putchar(i / 0);
    //puts((uint8_t*)"Test this bitch");
    while(1);
}
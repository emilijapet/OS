#include "types.h"
#include "alloc.h"
#include "multiboot2.h"
#include "scrn.h"

void append_to_page_table(page_table_t *table, page_t entry){
    table->entries[table->last_entry] = entry;
    table->last_entry += 1;
}

int table_full(page_table_t *table){
    return table->last_entry == 512 ? 1 : 0;
}

void create_table_entries(multiboot_memory_map_t *mmap){
    uint64_t i;
    uint64_t current_entry_end = mmap->addr + mmap->len;

    // Starting from the physical address of the memory chunk this entry points to,
    // go through the chunk in 4KB steps

    for(i = mmap->addr; i + PAGE_SIZE < current_entry_end; i += PAGE_SIZE){
        // Check if our L1 table is full or not
        if(table_full(&pt_c)){
            page_table_t new_pt;
            new_pt.last_entry = 0;

            page_t pt_entry;
            pt_entry.physical_addr = (uint64_t) &new_pt;

            if(table_full(&pdt_c)){
                page_table_t new_pdt;
                new_pdt.last_entry = 0;

                page_t pdt_entry;
                pdt_entry.physical_addr = (uint64_t) &new_pdt;

                if(table_full(&pdpt_c)){
                    page_table_t new_pdpt;
                    new_pdpt.last_entry = 0;

                    page_t pdpt_entry;
                    pdpt_entry.physical_addr = (uint64_t) &new_pdpt;

                    // Append it to the PML4 table
                    append_to_page_table(&pml4_c, pdpt_entry);

                    // Set PDPT to the new table
                    pdpt_c = new_pdpt;
                }

                // Append it to the PDPT table
                append_to_page_table(&pdpt_c, pdt_entry);
                pdt_c = new_pdt;
            }

            append_to_page_table(&pdt_c, pt_entry);
            pt_c = new_pt;
        }

        page_t pt_entry;
        pt_entry.physical_addr = i;

        append_to_page_table(&pt_c, pt_entry);
    }
}

uint64_t map_memory(unsigned long addr){//unsigned long magic,
    struct multiboot_tag *tag;
    uint64_t total_memory_size = 0;

    // Initialize table entries for all page tables
    page_t pt_entry, pdt_entry, pdpt_entry;
    pt_entry.physical_addr = &pt_c;
    pdt_entry.physical_addr = &pdt_c;
    pdpt_entry.physical_addr = &pdpt_c;

    pdt_c.entries[pdt_c.last_entry] = pt_entry;
    pdpt_c.entries[pdpt_c.last_entry] = pdt_entry;
    pml4_c.entries[pml4_c.last_entry] = pdpt_entry;

    pdt_c.last_entry += 1;
    pdpt_c.last_entry += 1;
    pml4_c.last_entry += 1;

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
                    // We found an entry with available memory, create mappings for the 4KB page frames
                    create_table_entries(mmap);
                    total_memory_size += mmap->len;
                }
            }
        }
    }
    printf("%d", pdt_c.last_entry);
    tag = (struct multiboot_tag *) ((multiboot_uint8_t *) tag + ((tag->size + 7) & ~7));
    return total_memory_size;
}
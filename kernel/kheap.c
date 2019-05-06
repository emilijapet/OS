#include "sys.h"
#include "kheap.h"
//#include "types.h"
#include "paging.h"

static int8_t header_less_than(void *a, void *b){
    return (((header_t *) a)->size < ((header_t *) b)->size) ? 1 : 0;
}

static int64_t find_smallest_hole(uint64_t size, uint8_t page_align, heap_t *heap){
    uint64_t iterator = 0;

    while(iterator < heap->index.size){
        header_t *header = (header_t *) find_ordered_list(iterator, &heap->index);
        if(page_align > 0){
            // Page align the starting point of the header
            uint64_t location = (uint64_t) header;
            int64_t offset = 0;

            if((location + sizeof(header_t)) & 0xFFFFFFFFFF000000 != 0){
                offset = 0x1000 - (location + sizeof(header_t)) % 0x1000;
            }

            int64_t hole_size = (int64_t) header->size - offset;

            // End the loop if the size of the hole is fine now or we have found
            // a header with an appropriate size
            if(hole_size >= (int64_t) size){
                break;
            } else if(header->size >= size){
                break;
            }

            iterator++;
        }

        // If we reached the end of the heap and didn't find anything, return -1
        if(iterator == heap->index.size){
            return -1;
        }
        return iterator;
    }
}

static void expand_heap(uint64_t new_size, heap_t *heap){
    uint64_t old_size = heap->end_address - heap->start_address;

    // Make sure the requirements make sense
    ASSERT(new_size > old_size);

    // Get the nearest following page boundary
    if(new_size & 0xFFFFFFFFFF000000 != 0){
        new_size &= 0xFFFFFFFFFF000000;
        new_size += 0x1000;
    }

    // Make sure we're within the boundaries
    ASSERT(heap->start_address + new_size <= heap->max_address);

    uint64_t i = old_size;
    while(i < new_size){
        alloc_frame(get_page(heap->start_address + i, 1, kernel_directory), (heap->supervisor) ? 1 : 0, (heap->read_only) ? 0 : 1);
        i += 0x1000;
    }
    heap->end_address = heap->start_address + new_size;
}

heap_t *create_heap(uint64_t start, uint64_t end, uint64_t max, uint8_t supervisor, uint8_t read_only){
    heap_t *heap = (heap_t *) kmalloc(sizeof(heap_t));

    // Check the assumption that the addresses are page aligned
    ASSERT(start % 0x1000 == 0);
    ASSERT(end % 0x1000 == 0);

    // Initialize index and shift start address forward
    heap->index = place_ordered_list((void *) start, 0x20000, &header_less_than);
    start += sizeof(type_t) * 0x20000;

    // Make sure the start address is page aligned
    if(start & 0xFFFFFFFFFF000000 != 0){
        start &= 0xFFFFFFFFFF000000;
        start += 0x1000;
    }

    // Write the addresses to the heap
    heap->start_address = start;
    heap->end_address = end;
    heap->max_address = max;
    heap->supervisor = supervisor;
    heap->read_only = read_only;

    // Initialize a hole in the index
    header_t *hole = (header_t *) start;
    hole->size = end - start;
    hole->magic = HEAP_MAGIC;
    hole->free = 1;
    insert_ordered_list((void *) hole, &heap->index);

    return heap;
}

void *alloc(uint64_t size, uint8_t page_align, heap_t *heap){
    uint64_t new_size = size + sizeof(header_t) + sizeof(footer_t);
    int64_t iterator = find_smallest_hole(new_size, page_align, heap);

    // Do stuff if we didn't find enough memory
    if(iterator == -1){
        uint64_t old_length = heap->end_address - heap->start_address;
        uint64_t old_end_address = heap->end_address;

        // Allocate some more space
        expand_heap(old_length + new_size, heap);
        uint64_t new_length = heap->end_address - heap->start_address;

        iterator = 0;
        uint64_t index = -1, value = 0x0;

        while(iterator < heap->index.size){
            uint64_t tmp = (uint64_t) find_ordered_list(iterator, &heap->index);

            if(tmp > value){
                value = tmp;
                index = iterator;
            }

            iterator++;
        }

        // If we didn't find any headers, add one
        if(index == -1){
            header_t *header = (header_t *) old_end_address;
            header->magic = HEAP_MAGIC;
            header->size = new_length - old_length;
            header->free = 1;
            
            footer_t *footer = (footer_t *) (old_end_address + header->size - sizeof(footer_t));
            footer->magic = HEAP_MAGIC;
            footer->header = header;

            insert_ordered_list((void *) header, &heap->index);
        } else {
            header_t *header = find_ordered_list(index, &heap->index);
            header->size += new_size - old_length;

            footer_t *footer = (footer_t *) ((uint64_t) header + header->size - sizeof(footer_t));
            footer->magic = HEAP_MAGIC;
            footer->header = header;
        }

        // Call function recursively once enough space has been allocated on the heap
        return alloc(size, page_align, heap);
    }

    header_t *original_hole_header = (header_t *) find_ordered_list(iterator, &heap->index);
    uint64_t original_hole_position = (uint64_t) original_hole_header;
    uint64_t original_hole_size = original_hole_header->size;

    // If the differene between the old and new hole sizes is smaller than the overhead of a
    // header and a footer, don't split. Just use the entire hole.
    if(original_hole_size - new_size < sizeof(header_t) + sizeof(footer_t)){
        size += original_hole_size - new_size;
        new_size = original_hole_size;
    }

    // Page align if needed
    if(page_align && original_hole_position & 0xFFFFFFFFFF000000){
        uint64_t new_location = original_hole_position + 0x1000 - (original_hole_position & 0xFFFFFF) - sizeof(header_t);
        
        header_t *hole_header = (header_t *) original_hole_position;
        hole_header->magic = HEAP_MAGIC;
        hole_header->size = 0x1000 - (original_hole_position & 0xFFFFFF) - sizeof(header_t);
        hole_header->free = 1;

        footer_t *hole_footer = (footer_t *) ((uint64_t) new_location - sizeof(footer_t));
        hole_footer->magic = HEAP_MAGIC;
        hole_footer->header = hole_header;

        original_hole_position = new_location;
        original_hole_size -= hole_header->size;
    } else {
        // Delete the hole from the index
        remove_ordered_list(iterator, &heap->index);
    }

    // Overwrite the original header and footer
    header_t *block_header = (header_t *) original_hole_position;
    block_header->magic = HEAP_MAGIC;
    block_header->size = new_size;
    block_header->free = 1;

    footer_t *block_footer = (footer_t *) (original_hole_position + sizeof(header_t) + size);
    block_footer->magic = HEAP_MAGIC;
    block_footer->header = block_header;

    // If needed, add another hole into the index
    if(original_hole_size - new_size > 0){
        header_t *hole_header = (header_t *) (original_hole_position + sizeof(header_t) + size + sizeof(footer_t));
        hole_header->magic = HEAP_MAGIC;
        hole_header->size = original_hole_size - new_size;
        hole_header->free = 1;

        footer_t *hole_footer = (footer_t *) ((uint64_t) hole_header + original_hole_size - new_size - sizeof(footer_t));
        
        // If the footer is still in range, finalize it
        if((uint64_t) hole_footer < heap->end_address){
            hole_footer->magic = HEAP_MAGIC;
            hole_footer->header = hole_header;
        }

        insert_ordered_list((void *) hole_header, &heap->index);
    }

    // Finally return
    return (void *) ((uint64_t) block_header + sizeof(header_t));
}
void free(void *p, heap_t *heap);
uint64_t kmalloc_internal(uint64_t size, int align, uint64_t *physical_location);
uint64_t kmalloc_aligned(uint64_t size);
uint64_t kmalloc_physical(uint64_t size, uint64_t *physical_location);
uint64_t kmalloc_aligned_physical(uint64_t size, uint64_t *physical_location);
uint64_t kmalloc(uint64_t size);
void kfree(uint64_t *p);








// uint64_t kmalloc(uint64_t size, int align, uint64_t *physical_addr){
//     // If the address isn't page aligned yet, do that
//     if(align == 1 && placement_addr & 0xFFFFF000){
//         placement_addr &= 0xFFFFF000;
//         placement_addr += 0x1000;
//     }

//     // If we're given a physical address, save the placement in it
//     if(physical_addr){
//         *physical_addr = placement_addr;
//     }

//     uint64_t tmp = placement_addr;
//     placement_addr += size;
//     return tmp;
// }
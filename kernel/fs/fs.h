#ifndef _FS_H_
#define _FS_H_

#include "../types.h"

#define BLOCK_SIZE 0x1000
#define GRANULARITY 0x40000000

typedef struct super_block {
    uint64_t magic_number;
    uint64_t reserved_area_size;// the size of the reserved area
    uint64_t data_block_size;   // the size of a single data block
    uint64_t num_blocks;        // the number of available data blocks
    uint64_t index_size;        // the size of the inode index table
    uint64_t num_inodes;        // the total number of inodes in the system
    uint64_t next_free_inode;   // the index of the next free inode
} super_block_t;

typedef struct inode {
    uint64_t valid;         // marks whether the inode is created
    uint64_t size;          // the total size of the file this inode points to
    uint64_t starting_address;  // the starting address of the file
} inode_t;

// save the super block and the first inode block here. The super block should
// be the very first entry in the disk, followed by any other reserved data, followed by the
// inode blocks.
super_block_t *super;
inode_t[] *inode_index_table;

void initialise_fs(uint64_t memory_size, uint64_t reserved_memory_size){
    super = 0x0;
    super->magic_number = 0xf0f03410;

    // save the size of the reserved area
    super->reserved_area_size = reserved_memory_size;
    
    // save the size of a data block
    super->data_block_size = BLOCK_SIZE;

    // get the total number of blocks available, then see how many inodes should be initialized
    // and how many data blocks are left for use
    uint64_t available_memory_blocks = (memory_size - sizeof(super_block_t) - reserved_memory_size) / BLOCK_SIZE;
    super->num_inodes = (BLOCK_SIZE / GRANULARITY) * available_memory_blocks;
    super->index_size = super->num_inodes * sizeof(inode_t);
    super->num_blocks = available_memory_blocks - (super->index_size / BLOCK_SIZE);

    // finally, the next free inode will be the first inode in the index
    super->next_free_inode = 0;

    // then initialize the index table
    inode_index_table = (inode_t *) (memory_size - super->index_size);
    memset(inode_index_table, 0, super->index_size);
}

#endif
#ifndef _FS_H_
#define _FS_H_

#include "../types.h"

#define POINTERS_PER_NODE 5 // TODO: verify!
#define INODES_PER_BLOCK 64
#define POINTERS_PER_BLOCK 512
#define BLOCK_SIZE 0x1000   // TODO: verify!

typedef struct super_block {
    uint64_t magic_number;  // the magic number, 0xf0f03410
    uint64_t num_blocks;    // the number of data blocks
    uint64_t num_iblocks;   // the number of inode blocks (10% of data blocks)
    uint64_t num_inodes;    // the number of inodes
} super_block_t;

typedef struct inode {
    uint64_t valid;         // marks whether the inode is created
    uint64_t size;          // the total size of the file this inode points to
    uint64_t starting_address;  // the starting address of the file
} inode_t;

typedef struct inode_block {
    inode__t inodes[INODES_PER_BLOCK];  // all the inodes of this block
} inode_block__t;

// save the super block and the first inode block here. The super block should
// be the very first entry in the disk, followed by any other reserved data, followed by the
// inode blocks.
super_block_t super;
inode_block__t first_iblock;

void initialise_fs(uint64_t memory_size, uint64_t reserved_memory_size){
    &super = 0x0;
    super.magic_number = 0xf0f03410;

    // 90% of the total available memory will be the number of data blocks, 10% will be the inode blocks
    uint64_t available_memory_blocks = (memory_size - reserved_memory_size - sizeof(super_block_t)) / BLOCK_SIZE;
    super.num_blocks = available_memory_blocks * 0.9;
    super.num_iblocks = available_memory_blocks * 0.1;
    super.num_inodes = super.super.num_iblocks * POINTERS_PER_NODE;

    // set the location for the first iblock
    &first_iblock = sizeof(super_block_t) + reserved_memory_size;
}

#endif
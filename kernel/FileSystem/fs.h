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
    uint64_t direct[POINTERS_PER_NODE]; // direct pointers to data blocks
    uint64_t indirect;      // pointer to an indirect pointer block
} inode_t;

typedef struct block {
    super_block_t super;       // the data of a superblock
    inode_t inodes[INODES_PER_BLOCK]; // the data of an inode block
    uint64_t pointers[POINTERS_PER_BLOCK];  // the data of an indirect pointer block
    char data[BLOCK_SIZE];  // the data of a data block
} block_t;

#endif
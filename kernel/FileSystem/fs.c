#include "fs.h"
#include "../types.h"

int64_t first_free_inode(){
    inode_block_t *cur_iblock = first_iblock;
    inode_t inode;
    uint64_t i, inode_block_offset = 0;

    for(i = 0; i <= INODES_PER_BLOCK; i++){
        // move on to the next inode block and reset the index
        if(i == INODES_PER_BLOCK){
            i = 0;
            cur_iblock += sizeof(inode_block_t);
            inode_block_offset += 1;
        }

        inode = cur_iblock->inodes[i];
        
        if(!inode.valid){
            return i + inode_block_offset * INODES_PER_BLOCK;
        }
    }
    return -1;
}

inode_t find_inode(uint64_t inumber){
    uint64_t inode_block_offset = (int) (inumber / INODES_PER_BLOCK);
    inode_block_t *cur_iblock = first_iblock;
    cur_iblock += inode_block_offset * sizeof(inode_block_t);

    inode_t inode = cur_iblock->inodes[inumber - inode_block_offset * INODES_PER_BLOCK];

    // if(inode.valid){
    //     return inode;
    // }
    // return 0;
    return inode;
}

int64_t find_free_blocks(uint64_t num){
    // TODO: do the bitmap stuff
    return -1;
}

int64_t create_file(uint64_t size){
    uint64_t inumber = first_free_inode();
    uint64_t block_address = find_free_blocks((int) (size / BLOCK_SIZE));

    if(inumber && block_address){
        inode_t inode = find_inode(inumber);
        inode.valid = 1;
        inode.size = size;
        inode.starting_address = block_address;

        return inumber;
    }
    return -1;
}

int64_t write_file(uint64_t inumber, char *data, uint64_t size){
    inode_t inode = find_inode(inumber);
    int i;

    if(!inode.valid || size > inode.size){
        return -1;
    }
    // write the data to disk using the start_address
    return 0;
}

int64_t remove_file(uint64_t inumber){
    inode_t inode = find_inode(inumber);

    if(!inode.valid){
        return -1;
    }

    inode.valid = 0;
    inode.size = 0;
    inode.starting_address = 0;

    return 0;

}

char* read_file(uint64_t inumber){
    inode_t inode = find_inode(inumber);
    
    if(!inode.valid){
        return '\0';
    }

    // read the data
    return 0;
}

int main(){}
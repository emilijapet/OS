#include "fs.h"
#include "../types.h"
#include "../mem/memory_management.h"

int64_t next_free_inode(){
    uint64_t i;

    for(i = super->next_free_inode + 1; i < super->num_inodes; i++){
        if(!inode_index_table[i]->valid){
            super->next_free_inode = i;
            return 0;
        }
    }

    // if we reached the end of the index and didn't find any free inodes,
    // restart from the start of the table
    for(i = 0; i < super->next_free_inode; i++){
        if(!inode_index_table[i]->valid){
            super->next_free_inode = i;
            return 0;
        }
    }

    // if we haven't found any free inodes at all, return an error
    return -1;
}

inode_t * find_inode(uint64_t inumber){
    inode_t *inode = inode_index_table[inumber];
    return inode;
}

int64_t create_file(uint64_t size){
    uint64_t inumber = super->next_free_inode;
    uint64_t block_address = kmalloc(size);

    // if we have a valid inumber and a block address, initialize the inode
    if(inumber && block_address){
        inode_t *inode = find_inode(inumber);
        inode->valid = 1;
        inode->size = size;
        inode->starting_address = block_address;

        // update the super block as well
        super->next_free_inode = next_free_inode();

        return inumber;
    }
    return -1;
}

int64_t write_file(uint64_t inumber, char *data, uint64_t size){
    inode_t *inode = find_inode(inumber);

    if(!inode->valid || size > inode->size){
        return -1;
    }
    // write the data to disk using the start_address
    return 0;
}

int64_t delete_file(uint64_t inumber){
    inode_t *inode = find_inode(inumber);

    if(!inode->valid){
        return -1;
    }

    kfree(inode->starting_address);

    inode->valid = 0;
    inode->size = 0;
    inode->starting_address = 0;

    return 0;

}

char* read_file(uint64_t inumber){
    inode_t *inode = find_inode(inumber);
    
    if(!inode->valid){
        return '\0';
    }

    // read the data
    return 0;
}
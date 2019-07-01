#include "fs.h"
#include "../types.h"

int64_t first_free_inode(){
    inode_block__t cur_iblock = first_iblock;
    inode__t inode;
    uint64_t i, inode_block_offset = 0;

    for(i = 0; i <= INODES_PER_BLOCK; i++){
        // move on to the next inode block and reset the index
        if(i == INODES_PER_BLOCK){
            i = 0;
            &cur_iblock += sizeof(inode_block__t);
            inode_block_offset += 1;
        }

        inode = cur_iblock.inodes[i];
        
        if(!inode.valid){
            return i + inode_block_offset * INODES_PER_BLOCK;
        }
    }
    return -1;
}

inode_t find_inode(uint64_t inumber){
    uint64_t inode_block_offset = (int) (inumber / INODES_PER_BLOCK);
    inode_block__t cur_iblock = first_iblock;
    &cur_iblock += inode_block_offset * sizeof(inode_block__t);

    inode__t inode = cur_iblock.inodes[inumber - inode_block_offset * INODES_PER_BLOCK];

    if(inode.valid){
        return inode;
    }
    return -1;
}

int64_t find_free_blocks(uint64_t num){
    // TODO: do the bitmap stuff
    return -1;
}

int64_t create_file(uint64_t size){
    uint64_t inumber = first_free_inode();
    uint64_t block_address = find_free_blocks((int) (size / BLOCK_SIZE));

    if(inumber && block_address){
        inode__t inode = find_inode(inumber);
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

    if(size > inode.size){
        return -1;
    }
    // write the data to disk using the start_address
    return 0;
}

int64_t empty_file(uint64_t inumber){
    inode__t inode = find_inode(inumber);

    if(!inode){
        return -1;
    }

    int i = 0, found_empty_block = 0;
    block_t data_block;

    // write all zeroes for the size of the file using the starting address

    // do {
    //     if(i < POINTERS_PER_NODE){
    //         if(inode.direct[i]){
    //             memset(inode.direct[i], 0, BLOCK_SIZE);
    //         } else {
    //             found_empty_block = 1;
    //         }
    //     } else {
    //         if(inode.indirect[i - POINTERS_PER_NODE]){
    //             memset(inode.indirect[i - POINTERS_PER_NODE], 0, BLOCK_SIZE);
    //         } else {
    //             found_empty_block = 1;
    //         }
    //     }
    //     i++;
    // } while(!found_empty_block);

    return 0;
}

int64_t remove_file(uint64_t inumber){
    int64_t err_code = empty_file(inumber);

    if(!err_code){
        return -1;
    }
    
    inode__t inode = find_inode(inumber);
    inode.valid = 0;
    inode.size = 0;
    inode.starting_address = 0;

    return 0;

}

char* read_file(uint64_t inumber){
    inode__t inode = find_inode(inumber);
    // read the data
    return 0;
}

int main(){}
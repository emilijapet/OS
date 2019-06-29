#include "fs.h"
#include "../types.h"

int64_t first_free_inode(){
    block__t cur_iblock = first_iblock;
    inode__t inode;
    uint64_t i, inode_block_offset = 0;

    for(i = 0; i <= INODES_PER_BLOCK; i++){
        // move on to the next inode block and reset the index
        if(i == INODES_PER_BLOCK){
            i = 0;
            &cur_iblock += sizeof(block__t);
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
    block__t cur_iblock = first_iblock;
    &cur_iblock += inode_block_offset * sizeof(block__t);

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

        for(int i = 0; i < size / BLOCK_SIZE; i++){
            if(i >= POINTERS_PER_NODE){
                inode.indirect[i - POINTERS_PER_NODE] = block_address;
            } else {
                inode.direct[i] = block_address;
            }
            block_address += BLOCK_SIZE;
        }

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

    for(i = 0; i < sizeof(data) / BLOCK_SIZE; i++){
        if(i < POINTERS_PER_NODE){
            // write the direct blocks
        } else {
            // write the indirect blocks
        }
    }
    return 0;
}

int64_t empty_file(uint64_t inumber){
    inode__t inode = find_inode(inumber);

    if(!inode){
        return -1;
    }

    int i = 0, found_empty_block = 0;
    block_t data_block;

    do {
        if(i < POINTERS_PER_NODE){
            if(inode.direct[i]){
                memset(inode.direct[i], 0, BLOCK_SIZE);
            } else {
                found_empty_block = 1;
            }
        } else {
            if(inode.indirect[i - POINTERS_PER_NODE]){
                memset(inode.indirect[i - POINTERS_PER_NODE], 0, BLOCK_SIZE);
            } else {
                found_empty_block = 1;
            }
        }
        i++;
    } while(!found_empty_block);

    return 0;
}

int64_t remove_file(uint64_t inumber){
    int64_t err_code = empty_file(inumber);

    if(!err_code){
        return -1;
    }
    
    inode__t inode = find_inode(inumber);
    int i;

    for(i = 0; i < POINTERS_PER_NODE; i++){
        inode.direct[i] = 0;
    }
    inode.indirect = 0;
    inode.valid = 0;

    return 0;

}

char* read_file(uint64_t inumber){
    inode__t inode = find_inode(inumber);
    return 0;
}

int main(){}
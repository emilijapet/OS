#ifndef _IDE_H_
#define _IDE_H_

#include "../types.h"

#define SECTSIZE 512

int ide_init(void);
uint32_t ide_num_sectors(void);
int ide_is_ready(void);
void ide_start_readwrite(uint32_t secno, size_t nsectors, int iswrite);

static inline void ide_start_read(uint32_t secno, size_t nsectors){
    ide_start_readwrite(secno, nsectors, 0);
}

static inline void ide_start_write(uint32_t secno, size_t nsectors){
    ide_start_readwrite(secno, nsectors, 1);
}

void ide_read_sector(char *dst);
void ide_write_sector(char *src);


#endif
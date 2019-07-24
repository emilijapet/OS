// PIO based driver code

#include "asm.h"
#include "ide.h"
#include "../types.h"
#include "../err.h"

// io registers used to communicate witht the disk controller
#define IDE_REG_BASE    0x1F0
#define IDE_REG_DATA    (IDE_REG_BASE + 0x0)
#define IDE_REG_SECCNT  (IDE_REG_BASE + 0x2)
#define IDE_REG_LBA_LO  (IDE_REG_BASE + 0x3)
#define IDE_REG_LBA_MID (IDE_REG_BASE + 0x4)
#define IDE_REG_LBA_HI  (IDE_REG_BASE + 0x5)
#define IDE_REG_DRIVE   (IDE_REG_BASE + 0x6)
#define IDE_REG_COMMAND (IDE_REG_BASE + 0x7)    // when writing
#define IDE_REG_STATUS  (IDE_REG_BASE + 0x7)    // when reading

// IDE_REG_STATUS statusses when reading
#define IDE_BSY         0x80
#define IDE_RDY         0x40
#define IDE_DF          0x20
#define IDE_ERR         0x01

// IDE_REG_COMMAND commands when writing
#define IDE_COM_READ    0x20
#define IDE_COM_WRITE   0x30
#define IDE_COM_IDENTIFY 0xEC

// this variable tracks which disk to use; 0 = master, 1 = slave
static int diskno = 1;

// this variable will hold the number of sectors on the disk
static uint32_t disk_sectors;

// check if the drive is ready
int ide_is_ready(){
    return (inb(IDE_REG_STATUS) & (IDE_BSY | IDE_RDY)) == IDE_RDY;
}

// wait for the disk to be ready
static int ide_wait_ready(bool check_error){
    int r;
    
    while(((r = inb(IDE_REG_STATUS)) & (IDE_BSY | IDE_RDY)) != IDE_RDY);

    if(check_error && (r & (IDE_DF | IDE_ERR)) != 0){
        return -1;
    }
    return 0;
}

static bool ide_probe_disk1(){
    int r, x;

    // wait for device 0 - the master disk - to be ready
    ide_wait_ready(0);

    // switch to device 1
    outb(IDE_REG_DRIVE, 0xE0 | (1 << 4));

    // this device has to be ready for a long time
    for(x = 0; x < 1000 && ((r = inb(IDE_REG_STATUS)) & (IDE_BSY | IDE_DF | IDE_ERR)) != 0; x++);

    // switch back to device 0
    outb(IDE_REG_DRIVE, 0xE0 | (0 << 4));

    // return whether device 1 was ready for long enough
    return x < 1000;

}

static void ide_set_disk(int d){
    if(d != 0 && d != 1){
        crash("Bad disk number");
    }

    diskno = d;
}

// start the read/write operation to disk for a given number of secotrs
void ide_start_readwrite(uint32_t secno, size_t nsectors, int iswrite){
    int r;

    // make sure the write is for less than 256 sectors
    if(nsectors > 256){
        crash("Too many sectors to write");
    }

    // wait until the disk is ready
    ide_wait_ready(0);

    // then put the details of the command into the register
    outb(IDE_REG_SECCNT, nsectors);
    outb(IDE_REG_LBA_LO, secno & 0xFF);
    outb(IDE_REG_LBA_MID, (secno >> 8) & 0xFF);
    outb(IDE_REG_LBA_HI, (secno >> 16) & 0xFF);
    outb(IDE_REG_DRIVE, 0xE0 | ((diskno & 1) << 4) | ((secno >> 24) & 0x0F));
    outb(IDE_REG_COMMAND, iswrite ? IDE_COM_WRITE : IDE_COM_READ);
}

// read a single sector?
void ide_read_sector(char *dst){
    insl(IDE_REG_BASE, dst, SECTSIZE / 4);
}

// write a single sector?
void ide_write_sector(char *src){
    outsl(IDE_REG_BASE, src, SECTSIZE / 4);
}

// retrieve disk info
static void ide_identify(){
    uint16_t info[256];

    // wait until the disk is ready
    ide_wait_ready(0);

    // put the details of the command in the register
    outb(IDE_REG_SECCNT, 0);
    outb(IDE_REG_LBA_LO, 0);
    outb(IDE_REG_LBA_MID, 0);
    outb(IDE_REG_LBA_HI, 0);
    outb(IDE_REG_DRIVE, 0xA0 | ((diskno & 1) << 4));
    outb(IDE_REG_COMMAND, IDE_COM_IDENTIFY);

    if(ide_wait_ready(1)){
        crash("Error during disk identify");
    }

    ide_read_sector((char *) info);
    disk_sectors = *((uint32_t *) &info[60]);
    // maybe print the stuff
}

// get the number of sectors
uint32_t ide_num_sectors(){
    return disk_sectors;
}

// initialize the driver
int ide_init(){
    if(!ide_probe_disk1()){
        crash("Could not find disk 1");
    }

    ide_set_disk(1);
    ide_identify();
    return 0;
}
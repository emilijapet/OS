# OS
This is an implementation of a 64-bit kernel with a file system specialized in writing large files into memory fast.

## Kernel
The kernel includes implementations of:
* GDT
* IDT
* Paging
* Memory mapping (kmalloc/kfree)

## File System
The file system is a combination of the SFS and UNIX file systems.
The connection to the IDE driver was not fully set up.

## Run
Compile and run the OS by executing `make qemu`.
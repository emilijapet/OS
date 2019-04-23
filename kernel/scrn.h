#ifndef __SCRN_H
#define __SCRN_H

#include "types.h"

extern  void clear();
extern void putchar(uint8_t c);
extern void puts(uint8_t *str);
extern void vga_init();
void printf (const char *format, ...);
void itoa (char *buf, int base, int d);

#endif

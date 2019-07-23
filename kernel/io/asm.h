#ifndef _INLINE_ASM_H_
#define _INLINE_ASM_H_

#include "../types.h"

static inline uint8_t inb(uint16_t port){
    uint8_t data;
    asm volatile("inb %w1, %0" : "=a" (data) : "d" (port));
    return data;
}

static inline void insl(uint16_t port, void *addr, size_t n){
    asm volatile(
        "cld\n"
        "repne\n"
        "insl\n" :
        "=D" (addr), "=c" (n) :
        "d" (port), "0" (addr), "1" (n) :
        "memory", "cc"
    );
}

static inline void outb(uint16_t port, uint8_t data){
    asm volatile("outb %0, %w1" :: "a" (data), "d" (port));
}

static inline void outsl(uint16_t port, const void *addr, size_t n){
    asm volatile(
        "cld\n"
        "repne\n"
        "outsl\n" :
        "=S" (addr), "=c" (n) :
        "d" (port), "0" (addr), "1" (n) :
        "cc"
    );
}

#endif
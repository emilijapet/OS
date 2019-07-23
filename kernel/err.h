#ifndef _ERR_H_
#define _ERR_H_

#include "io/scrn.h"

static inline void crash(char *msg){
    puts(msg);
    for(;;);
}

#endif
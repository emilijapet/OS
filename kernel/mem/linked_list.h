#ifndef _LINKED_H_
#define _LINKED_H_
#include "types.h"

typedef struct node {
    uint64_t previous_node;
    uint64_t starting_address;
    uint64_t size;
    uint64_t next_node;
} node_t;

node_t *head;

#endif
#ifndef _LINKED_H_
#define _LINKED_H_

typedef struct node {
    uint64_t previous_node;
    uint64_t starting_address;
    uint64_t size;
    uint64_t next_node;
} node__t;

#endif
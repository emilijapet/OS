#ifndef _ORDERED_LIST_H_
#define _ORDERED_LIST_H_

#include "types.h"

// Define an abstract type that could save any pointer
typedef void* type_t;
typedef int8_t (*lessthan_predicate_t)(type_t,type_t);
int8_t standard_lessthan_predicate(type_t a, type_t b);

typedef struct ordered_list {
    type_t *list;
    uint64_t size;
    uint64_t max_size;
    lessthan_predicate_t less_than;
} ordered_list_t;

// Functions for creating ordered lists
ordered_list_t create_ordered_list(uint64_t max_size, lessthan_predicate_t less_than);
ordered_list_t place_ordered_list(void *addr, uint64_t max_size, lessthan_predicate_t less_than);

// Function for destroying an ordered list
void destroy_ordered_list(ordered_list_t *list);

// Functions for adding, finding, deleting items in the list
void insert_ordered_list(type_t item, ordered_list_t *list);
type_t find_ordered_list(uint64_t index, ordered_list_t *list);
void remove_ordered_list(uint64_t index, ordered_list_t *list);

#endif
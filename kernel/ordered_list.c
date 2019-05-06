#include "ordered_list.h"

int8_t standard_lessthan_predicate(type_t a, type_t b){
    return (a<b)?1:0;
}

ordered_list_t create_ordered_list(uint64_t max_size, lessthan_predicate_t less_than){
    ordered_list_t result;

    result.list = (void *) kmalloc(max_size * sizeof(type_t));
    memset(result.list, 0, max_size * sizeof(type_t));
    result.size = 0;
    result.max_size = max_size;
    result.less_than = less_than;

    return result;
}

ordered_list_t place_ordered_list(void *addr, uint64_t max_size, lessthan_predicate_t less_than){
    ordered_list_t result;

    result.list = (type_t *) addr;
    memset(result.list, 0, max_size * sizeof(type_t));
    result.size = 0;
    result.max_size = max_size;
    result.less_than = less_than;

    return result;
}

void destroy_ordered_list(ordered_list_t *list){
    kfree(list->list);
}

void insert_ordered_list(type_t item, ordered_list_t *list){
    ASSERT(list->less_than);//less than

    uint64_t iterator = 0;

    while(iterator < list->size && list->less_than(list->list[iterator], item)){
        iterator++;
    }

    if(iterator == list->size){
        list->list[list->size++] = item;
    } else {
        type_t tmp = list->list[iterator];
        list->list[iterator] = item;

        while(iterator < list->size){
            iterator++;
            type_t tmp2 = list->list[iterator];
            list->list[iterator] = tmp;
            tmp = tmp2;
        }
        list->size++;
    }
}

type_t find_ordered_list(uint64_t index, ordered_list_t *list){
    ASSERT(index < list->size);
    return list->list[index];
}

void remove_ordered_list(uint64_t index, ordered_list_t *list){
    while(index < list->size){
        list->list[index] = list->list[index + 1];
        index++;
    }
    list->size--;
}
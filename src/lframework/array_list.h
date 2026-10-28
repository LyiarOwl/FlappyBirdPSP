#pragma once

#include "types.h"

typedef struct array_list
{
    usize element_size; // stride
    usize capacity;
    usize count;
    u8   *bytes;        // bytes
} Array_List;

Array_List array_list_new(usize capacity, usize element_size);

/**
 * adds the item to the end of the last item.
 * the bytes of the item is copied, which means
 * that allocating the object isn't necessary.
 * */
void array_list_push(Array_List *, const void *);

void *array_list_get_at(const Array_List *, usize id);

void array_list_free(Array_List *);

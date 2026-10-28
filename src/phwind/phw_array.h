#pragma once

#include "phw_types.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct phw_array
{
    usize element_size; // stride
    usize capacity;
    u8   *data;         // bytes;
} Phw_Array;

Phw_Array phw_array_create(usize capacity, usize element_size);

/*
 * a copy of the item is made internally.
 * don't use this to remove any item (by setting the
 * item to null for example), use `phw_array_remove_at` instead.
 * */
void phw_array_set_at(Phw_Array *, usize id, const void *item);

void *phw_array_get_at(const Phw_Array *, usize id);

void phw_array_remove_at(Phw_Array *, usize id);

void phw_array_destroy(Phw_Array *);

#ifdef __cplusplus
}
#endif

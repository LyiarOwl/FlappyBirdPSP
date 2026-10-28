#pragma once

#include "types.h"
typedef struct array
{
    usize element_size; // stride
    usize capacity;
    u8   *bytes;
} Array;

Array array_new(usize capacity, usize element_size);

/*
 * item is copied internally!
 */
void array_set(Array *, usize id, const void *item);

void *array_get(const Array *, usize id);

void array_free(Array *);

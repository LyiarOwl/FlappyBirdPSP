#include "array.h"
#include "common.h"
#include <stdbool.h>
#include <stdlib.h>

Array array_new(usize capacity, usize element_size)
{
    Array arr = { 0 };
    if (capacity == 0 || element_size == 0) {
        l_assert(false, "[>>] array_create: capacity or element_size is 0!");
        return arr;
    }

    u8 *ptr = calloc(capacity, element_size);
    if (!ptr) {
        l_assert(false, "[>>] array_create: unable to allocate the array");
        return arr;
    }

    arr.bytes        = ptr;
    arr.element_size = element_size;
    arr.capacity     = capacity;

    return arr;
}

void array_set(Array *array, usize id, const void *item)
{
    if (!array) {
        l_assert(false, "[>>] array_set: array reference is null!");
        return;
    }

    if (!array->bytes) {
        l_assert(false, "[>>] array_set: this array wasn't allocated!");
        return;
    }

    if (id >= array->capacity) {
        l_assert(false, "[>>] array_set: index out of bounds!");
        return;
    }

    if (!item) {
        l_assert(false, "[>>] array_set: item reference is null!!");
        return;
    }

    u8 *dest = array->bytes + id * array->element_size;
    memcpy(dest, item, array->element_size);
}

void *array_get(const Array *array, usize id)
{
    if (!array) {
        l_assert(false, "[>>] array_get: array reference is null!");
        return NULL;
    }

    if (!array->bytes) {
        l_assert(false, "[>>] array_get: this array wasn't allocated!");
        return NULL;
    }

    if (id >= array->capacity) {
        l_assert(false, "[>>] array_get: index out of bounds!");
        return NULL;
    }

    return array->bytes + id * array->element_size;
}

void array_free(Array *array)
{
    if (!array)
        return;

    if (!array->bytes)
        return;

    free(array->bytes);
    array->element_size = 0;
    array->capacity     = 0;
    array->bytes        = NULL;
}

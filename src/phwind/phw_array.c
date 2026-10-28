#include "phw_array.h"
#include "phw_common.h"
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

Phw_Array phw_array_create(usize capacity, usize element_size)
{
    Phw_Array arr = { 0 };

    if (capacity == 0 || element_size == 0) {
        phw_assert(
            false,
            "[>>] phw_array_create: capacity AND element_size must be > 0!"
        );
        return arr;
    }

    if (capacity > SIZE_MAX / element_size)
        return arr;

    u8 *ptr = calloc(capacity, element_size);
    if (!ptr)
        return arr; /* Memory pressure is expected on PSP; caller checks data. */

    arr.data         = ptr;
    arr.capacity     = capacity;
    arr.element_size = element_size;

    return arr;
}

/*
 * a copy of the item is made internally.
 * */
void phw_array_set_at(Phw_Array *array, usize id, const void *item)
{
    if (!array) {
        phw_assert(false, "[>>] phw_array_set_at: array refeference is null!");
        return;
    }

    if (!array->data || !item || id >= array->capacity) {
        phw_assert(false, "[>>] phw_array_set_at: index out of bounds!");
        return;
    }

    u8 *dest = array->data + id * array->element_size;
    memcpy(dest, item, array->element_size);
}

void *phw_array_get_at(const Phw_Array *array, usize id)
{
    if (!array) {
        phw_assert(false, "[>>] phw_array_get_at: array refeference is null!");
        return NULL;
    }

    if (!array->data || id >= array->capacity) {
        phw_assert(false, "[>>] phw_array_get_at: index out of bounds!");
        return NULL;
    }

    return array->data + id * array->element_size;
}

void phw_array_remove_at(Phw_Array *array, usize id)
{
    if (!array) {
        phw_assert(
            false, "[>>] phw_array_remove_at: array refeference is null!"
        );
        return;
    }

    if (!array->data || id >= array->capacity) {
        phw_assert(false, "[>>] phw_array_remove_at: index out of bounds!");
        return;
    }

    u8 *dest = array->data + id * array->element_size;
    memset(dest, 0, array->element_size);
}

void phw_array_destroy(Phw_Array *array)
{
    if (!array)
        return;

    if (!array->data)
        return;

    free(array->data);
    array->data         = NULL;
    array->capacity     = 0;
    array->element_size = 0;
}

#include "array_list.h"
#include "common.h"
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

Array_List array_list_new(usize capacity, usize element_size)
{
    Array_List list = { 0 };

    if (capacity == 0 || element_size == 0) {
        l_assert(
            false,
            "[>>] array_list_create: capacity or element size is zero bitch!"
        );
        return list;
    }

    u8 *ptr = calloc(capacity, element_size);
    if (!ptr) {
        l_assert(
            false, "[>>] array_list_create: unable to allocate the array list"
        );
        return list;
    }
    list.bytes        = ptr;
    list.capacity     = capacity;
    list.element_size = element_size;

    return list;
}

void array_list_push(Array_List *list, const void *item)
{
    if (!list) {
        l_assert(false, "[>>] array_list_push: list reference is null!");
        return;
    }

    if (!list->bytes) {
        l_assert(false, "[>>] array_list_push: this list wasn't allocated!");
        return;
    }

    if (list->count >= list->capacity) {
        l_assert(false, "[>>] array_list_push: list overflow!");
        return;
    }

    if (!item) {
        l_assert(false, "[>>] array_set: item reference is null!!");
        return;
    }

    u8 *dest = list->bytes + list->count * list->element_size;

    memcpy(dest, item, list->element_size);
    list->count++;
}

void *array_list_get_at(const Array_List *list, usize id)
{
    if (!list) {
        l_assert(false, "[>>] array_list_get: list reference is null!");
        return NULL;
    }

    if (!list->bytes) {
        l_assert(false, "[>>] array_list_get: this list wasn't allocated!");
        return NULL;
    }

    if (id >= list->count) {
        l_assert(false, "[>>] array_list_get: index out of bounds!");
        return NULL;
    }

    u8 *item = list->bytes + id * list->element_size;
    return item;
}

void array_list_free(Array_List *list)
{
    if (!list)
        return;

    if (!list->bytes)
        return;

    free(list->bytes);
    list->capacity     = 0;
    list->count        = 0;
    list->element_size = 0;
}

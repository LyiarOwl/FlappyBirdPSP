#include "observers_list.h"
#include "lframework/common.h"
#include <stdbool.h>
#include <stdlib.h>

Observers_List observers_list_new(usize max_capacity)
{
    Observers_List list = { 0 };
    if (max_capacity == 0) {
        l_assert(false, "[>>] observers_list_create: max capacity must be > 0");
        return list;
    }

    observer_cb *ptr = calloc(max_capacity, sizeof(observer_cb));
    if (!ptr) {
        l_assert(false, "[>>] observers_list_create: unable to allocate the observers list");
        return list;
    }

    list.observers = ptr;
    list.len       = max_capacity;

    return list;
}

void observers_list_notify_all(Observers_List *list, void *user_data)
{
    if (!list) {
        l_assert(false, "[>>] observers_list_notify_all: list reference is null!");
        return;
    }
    if (!list->observers) {
        l_assert(false, "[>>] observers_list_notify_all: this list wasn't allocated!");
        return;
    }

    for (usize i = 0; i < list->count; i++) {
        if (list->observers[i]) {
            list->observers[i](user_data);
        }
    }
}

void observers_list_add_observer(Observers_List *list, observer_cb callback)
{
    if (!list) {
        l_assert(false, "[>>] observers_list_add_observer: list reference is null!");
        return;
    }
    if (!list->observers) {
        l_assert(false, "[>>] observers_list_add_observer: this list wasn't allocated!");
        return;
    }
    if (list->count >= list->len) {
        l_assert(false, "[>>] observers_list_add_observer: list overflow!");
        return;
    }

    list->observers[list->count++] = callback;
}

void observers_list_free(Observers_List *list)
{
    if (!list)
        return;
    if (!list->observers)
        return;

    free(list->observers);
    list->observers = NULL;
    list->count     = 0;
    list->len       = 0;
}

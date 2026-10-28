#pragma once

#include "phwind/phw_types.h"

typedef void (*observer_cb)(void *user_data);

typedef struct observers_list
{
    usize        count;
    usize        len;
    observer_cb *observers;
} Observers_List;

Observers_List observers_list_new(usize max_capacity);

void observers_list_notify_all(Observers_List *, void *user_data);

void observers_list_add_observer(Observers_List *, observer_cb);

void observers_list_free(Observers_List *);

#pragma once

#include "phw_types.h"
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct phw_user_data
{
    const char *hint;
    void       *data;
    usize       data_size;
} Phw_User_Data;

#ifdef __cplusplus
}
#endif

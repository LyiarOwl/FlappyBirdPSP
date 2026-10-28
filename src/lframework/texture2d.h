#pragma once

#include "types.h"
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct texture2d
{
    int w;
    int h;
    u8 *data;
    bool available;
} Texture2D;

Texture2D texture2d_load(const char *filepath);

void texture2d_unload(Texture2D *texture);

#ifdef __cplusplus
}
#endif

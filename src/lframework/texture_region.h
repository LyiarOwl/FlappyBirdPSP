#pragma once

#include "rectangle.h"
#include "texture2d.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct texture_region
{
    const Texture2D *texture;
    Rectangle        clip_rect;
} Texture_Region;

Texture_Region texture_region_create(const Texture2D *, f32 x, f32 y, f32 w, f32 h);

Texture_Region texture_region_create_sub_region(const Texture_Region *, f32 x, f32 y, f32 w, f32 h);

#ifdef __cplusplus
}
#endif

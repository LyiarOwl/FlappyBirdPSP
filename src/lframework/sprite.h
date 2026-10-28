#pragma once

#include "colors.h"
#include "sprite_batch.h"
#include "texture_region.h"

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

#define spr_center_origin(spr) sprite_set_origin_norm(spr, 0.5f, 0.5f)


typedef struct sprite
{
    Texture_Region region;

    f32   x;
    f32   y;
    f32   angle;
    f32   origin_x;
    f32   origin_y;
    f32   scale_x;
    f32   scale_y;
    bool  flipX;
    bool  flipY;
    color color;
} Sprite;

Sprite sprite_new(Texture_Region);

void sprite_set_origin_norm(Sprite *, f32 x, f32 y);

void sprite_draw(Sprite *, Sprite_Batch *);

#ifdef __cplusplus
}
#endif

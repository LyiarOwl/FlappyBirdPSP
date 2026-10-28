#pragma once

#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct vec2
{
    f32 x;
    f32 y;
} Vec2;

f32 vec2_len_squared(Vec2);

Vec2 vec2_normalize(Vec2);

Vec2 vec2_dir(Vec2 from, Vec2 to);

Vec2 vec2_len_dir(f32 len, f32 dir);

#ifdef __cplusplus
}
#endif

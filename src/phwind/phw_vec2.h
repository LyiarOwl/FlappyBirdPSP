#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "phw_types.h"
#include <stdbool.h>

typedef struct phw_vec2
{
    f32 x;
    f32 y;
} Phw_Vec2;

static inline bool phw_vec2_equals(const Phw_Vec2 a, const Phw_Vec2 b)
{
    return a.x == b.x && a.y == b.y;
}

#ifdef __cplusplus
}
#endif

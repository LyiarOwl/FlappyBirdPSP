#pragma once

#include "phw_types.h"
#include <stdbool.h>

typedef struct phw_point
{
    i32 x;
    i32 y;
} Phw_Point;

static inline bool phw_point_equals(const Phw_Point a, const Phw_Point b)
{
    return a.x == b.x && a.y == b.y;
}

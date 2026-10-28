#pragma once

#include "phw_circle.h"
#include "phw_rect.h"
#include "phw_tile.h"
#include "phw_vec2.h"

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

#define PHW_EPSILON 0.0001f

bool phw_point_inside_slope(
    Phw_Vec2 point, Phw_Rect slope_bounds, Phw_Slope_Angle angle
);

Phw_Vec2 phw_get_circle_separation(Phw_Circle circle1, Phw_Circle circle2);

Phw_Vec2 phw_get_circle_rect_separation(Phw_Circle circle, Phw_Rect rect);

#ifdef __cplusplus
}
#endif

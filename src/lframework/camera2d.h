#pragma once

#include "vec2.h"
#include "types.h"
#include "rectangle.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct camera2d
{
    f32 x;
    f32 y;
    f32 offset_x;
    f32 offset_y;
    f32  zoom;
} Camera2D;

Camera2D camera2d_new(void);

Vec2 camera2d_screen_to_world(const Camera2D *, Vec2 position);

Vec2 camera2d_world_to_screen(const Camera2D *, Vec2 position);

void camera2d_clamp_to_bounds(Camera2D *, Rectangle bounds);

#ifdef __cplusplus
}
#endif

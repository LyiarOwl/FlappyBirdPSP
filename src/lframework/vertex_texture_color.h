#pragma once

#include "colors.h"
#include "vec2.h"
#include "vec3.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct vertex_texture_color
{
    Vec2  uv;
    color color;
    Vec3  position;
} Vertex_Texture_Color;

#ifdef __cplusplus
}
#endif

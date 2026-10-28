#include "colors.h"
#include "vec3.h"

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

typedef struct vertex_color
{
    color color;
    Vec3  position;
} Vertex_Color;

#ifdef __cplusplus
}
#endif

#pragma once

#include "phw_vec2.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef enum phw_coll_shape_type { PHW_COLL_SHAPE_RECT, PHW_COLL_SHAPE_CIRCLE } Phw_Coll_Shape_Type;

typedef struct phw_collision_shape
{
    Phw_Coll_Shape_Type type;

    union {
        struct
        {
            Phw_Vec2 h_size;
        } rect;

        float radius;
    } shape;
} Phw_Collision_Shape;

Phw_Collision_Shape phw_create_circle_coll_shape(float radius);

Phw_Collision_Shape phw_create_rect_coll_shape(Phw_Vec2 half_size);

#ifdef __cplusplus
}
#endif

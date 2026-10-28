#include "phw_shapes.h"

Phw_Collision_Shape phw_create_circle_coll_shape(float radius)
{
    // clang-format off
    return (Phw_Collision_Shape){
        .type         = PHW_COLL_SHAPE_CIRCLE,
        .shape.radius = radius
    };
    // clang-format on
}

Phw_Collision_Shape phw_create_rect_coll_shape(Phw_Vec2 half_size)
{
    // clang-format off
    return (Phw_Collision_Shape){
        .type = PHW_COLL_SHAPE_RECT,
        .shape.rect = {
            {
                half_size.x,
                half_size.y
            }
        }
    };
    // clang-format on
}

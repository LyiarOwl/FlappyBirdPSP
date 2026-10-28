#pragma once

#include "phw_vec2.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct phw_bbox
{
    Phw_Vec2 position;
    Phw_Vec2 h_size;
} Phw_Bbox;

f32 phw_bbox_get_left(const Phw_Bbox *);
f32 phw_bbox_get_top(const Phw_Bbox *);
f32 phw_bbox_get_right(const Phw_Bbox *);
f32 phw_bbox_get_bottom(const Phw_Bbox *);

#ifdef __cplusplus
}
#endif

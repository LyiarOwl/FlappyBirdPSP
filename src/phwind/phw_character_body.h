#pragma once

#include "phw_vec2.h"
#include "phw_bbox.h"
#include "phw_point.h"
#include "phw_shapes.h"
#include "phw_types.h"
#include "phw_user_data.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct phw_world Phw_World;

typedef struct phw_character_body
{
    Phw_World          *world;         // internal
    bool                enabled;
    bool                active;        // internal: slot is occupied
    bool                is_static;     // immutable placement; separate spatial grid
    u32                 user_layer_mask;
    u32                 last_query_id; // internal
    Phw_Point           bucket_start;  // internal
    Phw_Point           bucket_end;    // internal
    Phw_Bbox            bbox;
    Phw_Collision_Shape collision_shape;
    Phw_User_Data       user_data;
} Phw_Character_Body;

/* Static bodies cannot move via set_position: remove and create again. */
Phw_Vec2 phw_body_get_position(const Phw_Character_Body *);

/* Reports capacity failure instead of silently leaving the body at its old position. */
bool phw_body_try_set_position(Phw_Character_Body *, Phw_Vec2);
void phw_body_set_position(Phw_Character_Body *, Phw_Vec2);

#ifdef __cplusplus
}
#endif

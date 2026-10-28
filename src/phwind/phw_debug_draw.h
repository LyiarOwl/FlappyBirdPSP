#pragma once

#include "phw_bbox.h"
#include "phw_rect.h"
#include "phw_shapes.h"
#include "phw_user_data.h"
#include "phw_vec2.h"
#include "phw_world.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct phw_debug_draw_tile_data
{
    Phw_Vec2 position;
    float    tile_size;
    Phw_Tile tile;
} Phw_Debug_Draw_Tile_Data;

typedef struct phw_debug_draw_body_data
{
    Phw_Bbox            bbox;
    Phw_Collision_Shape collision_shape;
    bool                enabled;
} Phw_Debug_Draw_Body_Data;

typedef struct phw_debug_draw_bucket_data
{
    Phw_Vec2 position;
    Phw_Vec2 size;
} Phw_Debug_Draw_Bucket_Data;

typedef void (*phw_debug_draw_render_bodies_cb)(
    Phw_Debug_Draw_Body_Data data, Phw_User_Data userData
);
typedef void (*phw_debug_draw_render_tiles_cb)(
    Phw_Debug_Draw_Tile_Data data, Phw_User_Data userData
);
typedef void (*phw_debug_draw_render_buckets_under_bodies)(
    Phw_Debug_Draw_Bucket_Data data, Phw_User_Data userData
);
typedef void (*phw_debug_draw_render_buckets_cb)(
    Phw_Debug_Draw_Bucket_Data data, Phw_User_Data userData
);

typedef struct phw_debug_draw
{
    phw_debug_draw_render_bodies_cb            render_bodies;
    phw_debug_draw_render_tiles_cb             render_tiles;
    phw_debug_draw_render_buckets_under_bodies render_buckets_under_bodies;
    phw_debug_draw_render_buckets_cb           render_buckets_grid;
    Phw_User_Data                              user_data;
} Phw_Debug_Draw;

Phw_Debug_Draw phw_debug_draw_create(void);

void phw_debug_draw_process_data(
    const Phw_Debug_Draw *, const Phw_World *, const Phw_Rect *tiles_view_bounds
);

#ifdef __cplusplus
}
#endif

#pragma once

#include "phw_array.h"
#include "phw_rect.h"
#include "phw_bbox.h"
#include "phw_character_body.h"
#include "phw_shapes.h"
#include "phw_spatial_grid.h"
#include "phw_tile.h"
#include "phw_tilemap.h"
#include "phw_user_data.h"
#include "phw_vec2.h"

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum phw_pipeline_mask {
    PHW_PIPE_MASK_ALL             = 0,
    PHW_PIPE_MASK_TILES           = 1 << 0,
    PHW_PIPE_MASK_DYNAMIC_OBJECTS = 1 << 1,
    PHW_PIPE_MASK_STATIC_OBJECTS  = 1 << 2
} Phw_Pipeline_Mask;

typedef struct phw_world
{
    Phw_Spatial_Grid   spatial_grid; /* dynamic */
    Phw_Spatial_Grid   static_grid;  /* immutable static bodies */
    const Phw_Tilemap *tilemap;
    u32                current_query_id;
    usize              bodies_count;
    Phw_Array          bodies;        /* fixed, stable-address dynamic slots */
    usize              static_bodies_count;
    Phw_Array          static_bodies; /* fixed, stable-address static slots */

} Phw_World;

/* Legacy constructor preserves the old large pool sizing. Prefer create_ex. */
Phw_World phw_world_create(
    Phw_Rect bounds, i32 rows_divisions, i32 columns_divisions, usize max_capac_per_bucket
);

/* Preferred on PSP: configure real body capacities independently of bucket capacity.
   Set max_static_bodies to 0 to allocate no static grid/pool. Returns empty world
   on allocation failure (world.bodies.data == NULL). */
Phw_World phw_world_create_ex(
    Phw_Rect bounds, i32 rows_divisions, i32 columns_divisions,
    usize dynamic_bucket_capacity, usize static_bucket_capacity,
    usize max_dynamic_bodies, usize max_static_bodies
);

/* Reindexes active bodies after a deliberate clear (does NOT discard bodies). */
void phw_world_rebuild_spatial_grid(Phw_World *);

/**
    @param collisionShape use `PhwShapes.h` functions to quickly (and properly)
   create the shapes.
*/
Phw_Character_Body *
phw_world_create_character_body(Phw_World *, Phw_Collision_Shape collision_shape);

/* Explicit spawn position avoids temporarily crowding the origin bucket. */
Phw_Character_Body *phw_world_create_character_body_at(
    Phw_World *, Phw_Collision_Shape collision_shape, Phw_Vec2 position
);

/* Static bodies are indexed only when created; remove and recreate to move one. */
Phw_Character_Body *phw_world_create_static_body(
    Phw_World *, Phw_Collision_Shape collision_shape, Phw_Vec2 position
);

/* Removes from all occupied buckets and recycles its pool slot. The pointer
   becomes invalid and can subsequently refer to a different body. */
bool phw_world_remove_body(Phw_World *, Phw_Character_Body *);

bool phw_world_update_body(Phw_World *, Phw_Character_Body *);

bool phw_world_colliding_at(
    Phw_World *,
    Phw_Vec2                  position,
    Phw_Collision_Shape       collision_shape,
    bool                      do_precise_check,
    const Phw_Character_Body *ignore_body,
    Phw_Pipeline_Mask         pipeline_mask,
    u32                       user_layer_mask, /* optional */
    u8                        tile_type,
    u32                       user_tile_mask   /* optional */
);

/**
 * @param bbox center is considered the origin
 * @return the amount of bodies written in the buffer.
 */
usize phw_world_query_bodies(
    Phw_World *,
    usize                     internal_buckets_buffer_lockup_size,
    Phw_Bbox                  bbox,
    const Phw_Character_Body *bodies_buffer[],
    usize                     bodies_buffer_length
);

/* Optional extended query; default query_bodies remains dynamic-only. */
usize phw_world_query_bodies_with_mask(
    Phw_World *, Phw_Bbox bbox, const Phw_Character_Body *bodies_buffer[],
    usize bodies_buffer_length, Phw_Pipeline_Mask pipeline_mask
);

void phw_world_clear(Phw_World *);

/* Releases owned allocations but NOT the world struct itself (value-created). */
void phw_world_destroy(Phw_World *);

#ifdef __cplusplus
}
#endif

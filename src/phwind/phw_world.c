#include "phw_world.h"
#include "phw_collision.h"
#include "phw_common.h"
#include "phw_spatial_grid_bucket.h"
#include "phwind/phw_array.h"
#include "phwind/phw_spatial_grid.h"
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

static bool valid_shape(Phw_Collision_Shape shape)
{
    if (shape.type == PHW_COLL_SHAPE_RECT)
        return shape.shape.rect.h_size.x > 0.f && shape.shape.rect.h_size.y > 0.f;
    if (shape.type == PHW_COLL_SHAPE_CIRCLE)
        return shape.shape.radius > 0.f;
    return false;
}

static Phw_Vec2 half_size(Phw_Collision_Shape shape)
{
    if (shape.type == PHW_COLL_SHAPE_RECT)
        return shape.shape.rect.h_size;
    return (Phw_Vec2){ shape.shape.radius, shape.shape.radius };
}

Phw_World phw_world_create_ex(
    Phw_Rect bounds,
    i32      rows_divisions,
    i32      columns_divisions,
    usize    dynamic_bucket_capacity,
    usize    static_bucket_capacity,
    usize    max_dynamic_bodies,
    usize    max_static_bodies
)
{
    Phw_World world = { 0 };
    if (rows_divisions <= 0 || columns_divisions <= 0 || bounds.size.x <= 0.f ||
        bounds.size.y <= 0.f || !max_dynamic_bodies || !dynamic_bucket_capacity ||
        (max_static_bodies && !static_bucket_capacity))
        return world;

    world.bodies = phw_array_create(max_dynamic_bodies, sizeof(Phw_Character_Body));
    if (!world.bodies.data)
        return world;
    world.spatial_grid =
        phw_spatialgrid_create(bounds, rows_divisions, columns_divisions, dynamic_bucket_capacity);
    if (!world.spatial_grid.buckets_array.data)
        goto fail;

    if (max_static_bodies) {
        world.static_bodies = phw_array_create(max_static_bodies, sizeof(Phw_Character_Body));
        if (!world.static_bodies.data)
            goto fail;
        world.static_grid = phw_spatialgrid_create(
            bounds, rows_divisions, columns_divisions, static_bucket_capacity
        );
        if (!world.static_grid.buckets_array.data)
            goto fail;
    }
    return world;
fail:
    phw_world_destroy(&world);
    return (Phw_World){ 0 };
}

/* Legacy memory sizing; for PSP 32MB prefer create_ex. */
Phw_World phw_world_create(
    Phw_Rect bounds, i32 rows_divisions, i32 columns_divisions, usize max_capac_per_bucket
)
{
    if (rows_divisions <= 0 || columns_divisions <= 0 || !max_capac_per_bucket)
        return (Phw_World){ 0 };
    usize rows = (usize)rows_divisions, cols = (usize)columns_divisions;
    if (cols > SIZE_MAX / rows || cols * rows > SIZE_MAX / max_capac_per_bucket)
        return (Phw_World){ 0 };
    return phw_world_create_ex(
        bounds,
        rows_divisions,
        columns_divisions,
        max_capac_per_bucket,
        0,
        cols * rows * max_capac_per_bucket,
        0
    );
}

static Phw_Character_Body *
create_body(Phw_World *world, Phw_Collision_Shape shape, Phw_Vec2 position, bool is_static)
{
    if (!world || !valid_shape(shape))
        return NULL;
    Phw_Array        *slots = is_static ? &world->static_bodies : &world->bodies;
    usize            *count = is_static ? &world->static_bodies_count : &world->bodies_count;
    Phw_Spatial_Grid *grid  = is_static ? &world->static_grid : &world->spatial_grid;
    if (!slots->data || *count == slots->capacity) {
        if (*count == slots->capacity) {
            phw_assert(false, "[phw_world_create_body (internal)]: bodies overflow!");
        }
        return NULL;
    }

    /* Fixed slot addresses: bucket pointers never move, even after deletions. */
    for (usize i = 0; i < slots->capacity; ++i) {
        Phw_Character_Body *body = phw_array_get_at(slots, i);
        if (body->active)
            continue;
        *body                 = (Phw_Character_Body){ 0 };
        body->world           = world;
        body->enabled         = true;
        body->active          = true;
        body->is_static       = is_static;
        body->collision_shape = shape;
        body->bbox            = (Phw_Bbox){ position, half_size(shape) };
        body->bucket_start    = (Phw_Point){ -1, -1 };
        body->bucket_end      = (Phw_Point){ -1, -1 };
        if (!phw_spatialgrid_push(grid, body)) {
            *body = (Phw_Character_Body){ 0 };
            return NULL;
        }
        ++(*count);
        return body;
    }
    return NULL;
}

Phw_Character_Body *
phw_world_create_character_body(Phw_World *world, Phw_Collision_Shape collision_shape)
{
    return create_body(world, collision_shape, (Phw_Vec2){ 0, 0 }, false);
}

Phw_Character_Body *phw_world_create_character_body_at(
    Phw_World *world, Phw_Collision_Shape collision_shape, Phw_Vec2 position
)
{
    return create_body(world, collision_shape, position, false);
}

Phw_Character_Body *phw_world_create_static_body(
    Phw_World *world, Phw_Collision_Shape collision_shape, Phw_Vec2 position
)
{
    return create_body(world, collision_shape, position, true);
}

/* Check that a public body pointer actually belongs to one of this world's
   fixed pools. Integer comparisons avoid undefined pointer-order comparisons. */
static bool belongs_to_pool(const Phw_Array *pool, const Phw_Character_Body *body)
{
    if (!pool || !pool->data || !body || pool->element_size != sizeof(*body))
        return false;
    uintptr_t start   = (uintptr_t)pool->data;
    uintptr_t address = (uintptr_t)body;
    if (address < start)
        return false;
    uintptr_t offset = address - start;
    return offset / sizeof(*body) < pool->capacity && offset % sizeof(*body) == 0;
}

static bool belongs_to_world(const Phw_World *world, const Phw_Character_Body *body)
{
    if (!world)
        return false;
    return belongs_to_pool(&world->bodies, body) || belongs_to_pool(&world->static_bodies, body);
}

bool phw_world_remove_body(Phw_World *world, Phw_Character_Body *body)
{
    if (!belongs_to_world(world, body) || !body->active || body->world != world)
        return false;
    Phw_Spatial_Grid *grid  = body->is_static ? &world->static_grid : &world->spatial_grid;
    usize            *count = body->is_static ? &world->static_bodies_count : &world->bodies_count;
    phw_spatialgrid_remove(grid, body);
    *body = (Phw_Character_Body){ 0 };
    --(*count);
    return true;
}

bool phw_world_update_body(Phw_World *world, Phw_Character_Body *body)
{
    if (!belongs_to_pool(world ? &world->bodies : NULL, body) || !body->active ||
        body->world != world || body->is_static)
        return false;
    return phw_spatialgrid_push(&world->spatial_grid, body);
}

void phw_world_rebuild_spatial_grid(Phw_World *world)
{
    if (!world)
        return;
    phw_spatialgrid_clear_buckets(&world->spatial_grid);
    phw_spatialgrid_clear_buckets(&world->static_grid);
    for (usize i = 0; i < world->bodies.capacity; ++i) {
        Phw_Character_Body *body = phw_array_get_at(&world->bodies, i);
        if (body->active && !phw_spatialgrid_push(&world->spatial_grid, body))
            phw_assert(false, "[phwind] failed rebuilding dynamic grid");
    }
    for (usize i = 0; i < world->static_bodies.capacity; ++i) {
        Phw_Character_Body *body = phw_array_get_at(&world->static_bodies, i);
        if (body->active && !phw_spatialgrid_push(&world->static_grid, body))
            phw_assert(false, "[phwind] failed rebuilding static grid");
    }
}

static bool
overlaps_shapes(Phw_Vec2 a_pos, Phw_Collision_Shape a, Phw_Vec2 b_pos, Phw_Collision_Shape b)
{
    a_pos.x = phw_ffloor(a_pos.x);
    a_pos.y = phw_ffloor(a_pos.y);
    b_pos.x = phw_ffloor(b_pos.x);
    b_pos.y = phw_ffloor(b_pos.y);
    if (a.type == PHW_COLL_SHAPE_RECT) {
        Phw_Rect a_rect = {
            a_pos, { a.shape.rect.h_size.x * 2.f, a.shape.rect.h_size.y * 2.f }
        };
        if (b.type == PHW_COLL_SHAPE_RECT)
            return phw_rect_vs_rect(
                a_rect,
                (Phw_Rect){
                    b_pos, { b.shape.rect.h_size.x * 2.f, b.shape.rect.h_size.y * 2.f }
            }
            );
        return phw_circle_vs_rect((Phw_Circle){ b_pos, b.shape.radius }, a_rect);
    }
    Phw_Circle a_circle = { a_pos, a.shape.radius };
    if (b.type == PHW_COLL_SHAPE_CIRCLE)
        return phw_circle_vs_circle(a_circle, (Phw_Circle){ b_pos, b.shape.radius });
    return phw_circle_vs_rect(
        a_circle,
        (Phw_Rect){
            b_pos, { b.shape.rect.h_size.x * 2.f, b.shape.rect.h_size.y * 2.f }
    }
    );
}

/* Pixel-style slope: triangle occupies half of its cell (see debug_renderer). */
static bool slope_vs_rect(Phw_Rect rect, f32 tx, f32 ty, f32 size, u8 angle)
{
    const f32 l = rect.position.x - rect.size.x * .5f - tx;
    const f32 r = rect.position.x + rect.size.x * .5f - tx;
    const f32 t = rect.position.y - rect.size.y * .5f - ty;
    const f32 b = rect.position.y + rect.size.y * .5f - ty;
    switch (angle) {
    case PHW_SLOPE_ANGLE_UP_RIGHT:
        return r + b > size;
    case PHW_SLOPE_ANGLE_UP_LEFT:
        return b > l;
    case PHW_SLOPE_ANGLE_DOWN_RIGHT:
        return r > t;
    case PHW_SLOPE_ANGLE_DOWN_LEFT:
        return l + t < size;
    default:
        return false;
    }
}

static f32 dist_sq_segment(Phw_Vec2 p, Phw_Vec2 a, Phw_Vec2 b)
{
    f32 vx = b.x - a.x, vy = b.y - a.y;
    f32 k  = vx * vx + vy * vy;
    f32 t  = k > 0.f ? ((p.x - a.x) * vx + (p.y - a.y) * vy) / k : 0.f;
    t      = phw_fclamp(t, 0.f, 1.f);
    f32 dx = p.x - (a.x + t * vx), dy = p.y - (a.y + t * vy);
    return dx * dx + dy * dy;
}

static bool slope_vs_circle(Phw_Circle circle, f32 tx, f32 ty, f32 s, u8 angle)
{
    /* Work in tile-local coordinates. Triangle edge checks use squared distances. */
    Phw_Vec2 p = { circle.position.x - tx, circle.position.y - ty };
    Phw_Vec2 a, b, c;
    switch (angle) {
    case PHW_SLOPE_ANGLE_UP_RIGHT:
        a = (Phw_Vec2){ s, 0 };
        b = (Phw_Vec2){ s, s };
        c = (Phw_Vec2){ 0, s };
        break;
    case PHW_SLOPE_ANGLE_UP_LEFT:
        a = (Phw_Vec2){ 0, 0 };
        b = (Phw_Vec2){ s, s };
        c = (Phw_Vec2){ 0, s };
        break;
    case PHW_SLOPE_ANGLE_DOWN_RIGHT:
        a = (Phw_Vec2){ 0, 0 };
        b = (Phw_Vec2){ s, 0 };
        c = (Phw_Vec2){ s, s };
        break;
    case PHW_SLOPE_ANGLE_DOWN_LEFT:
        a = (Phw_Vec2){ 0, 0 };
        b = (Phw_Vec2){ s, 0 };
        c = (Phw_Vec2){ 0, s };
        break;
    default:
        return false;
    }
    f32 ab = (b.x - a.x) * (p.y - a.y) - (b.y - a.y) * (p.x - a.x);
    f32 bc = (c.x - b.x) * (p.y - b.y) - (c.y - b.y) * (p.x - b.x);
    f32 ca = (a.x - c.x) * (p.y - c.y) - (a.y - c.y) * (p.x - c.x);
    if ((ab >= 0 && bc >= 0 && ca >= 0) || (ab <= 0 && bc <= 0 && ca <= 0))
        return true;
    f32 d = dist_sq_segment(p, a, b);
    d     = phw_fmin(d, dist_sq_segment(p, b, c));
    d     = phw_fmin(d, dist_sq_segment(p, c, a));
    return d < circle.radius * circle.radius;
}

static bool overlaps_tiles(
    const Phw_Tilemap  *tmap,
    Phw_Vec2            position,
    Phw_Collision_Shape shape,
    bool                precise,
    u8                  tile_type,
    u32                 user_tile_mask
)
{
    if (!tmap || !tmap->tile_size || !tmap->rows || !tmap->columns)
        return false;
    Phw_Vec2 h = half_size(shape);
    position.x = phw_ffloor(position.x);
    position.y = phw_ffloor(position.y);
    /* End bounds conservatively cover edge cells; rect test removes false matches. */
    i32 sx = (i32)phw_ffloor((position.x - h.x) / tmap->tile_size);
    i32 sy = (i32)phw_ffloor((position.y - h.y) / tmap->tile_size);
    i32 ex = (i32)phw_ffloor((position.x + h.x) / tmap->tile_size);
    i32 ey = (i32)phw_ffloor((position.y + h.y) / tmap->tile_size);
    if (ex < 0 || ey < 0 || sx >= tmap->columns || sy >= tmap->rows)
        return false;
    sx            = phw_imax(0, sx);
    sy            = phw_imax(0, sy);
    ex            = phw_imin(tmap->columns - 1, ex);
    ey            = phw_imin(tmap->rows - 1, ey);
    Phw_Rect rect = {
        position, { h.x * 2.f, h.y * 2.f }
    };
    for (i32 y = sy; y <= ey; ++y) {
        for (i32 x = sx; x <= ex; ++x) {
            Phw_Tile tile = phw_tmap_get_Tile(tmap, (Phw_Point){ x, y });
            if (tile.type == PHW_TILETYPE_EMPTY ||
                (tile_type && !phw_hasAnyFlag(tile.type, tile_type)) ||
                (user_tile_mask && !phw_hasAnyFlag(tile.user_tile_mask, user_tile_mask)))
                continue;
            f32      tx   = (f32)x * tmap->tile_size;
            f32      ty   = (f32)y * tmap->tile_size;
            Phw_Rect cell = {
                { tx + tmap->tile_size * .5f, ty + tmap->tile_size * .5f },
                {            tmap->tile_size,            tmap->tile_size }
            };
            if (!phw_rect_vs_rect(rect, cell))
                continue;
            if (!precise)
                return true;
            if (tile.type == PHW_TILETYPE_SOLID) {
                if (shape.type == PHW_COLL_SHAPE_RECT ||
                    phw_circle_vs_rect((Phw_Circle){ position, shape.shape.radius }, cell))
                    return true;
            } else if (tile.type == PHW_TILETYPE_SLOPE) {
                if (shape.type == PHW_COLL_SHAPE_RECT
                        ? slope_vs_rect(rect, tx, ty, tmap->tile_size, tile.angle)
                        : slope_vs_circle(
                              (Phw_Circle){ position, shape.shape.radius },
                              tx,
                              ty,
                              tmap->tile_size,
                              tile.angle
                          ))
                    return true;
            }
        }
    }
    return false;
}

static void next_query(Phw_World *world)
{
    if (++world->current_query_id != 0)
        return;
    /* No stale matches when u32 wraps. This path runs once per ~4 billion queries. */
    world->current_query_id = 1;
    for (usize i = 0; i < world->bodies.capacity; ++i) {
        Phw_Character_Body *body = phw_array_get_at(&world->bodies, i);
        body->last_query_id      = 0;
    }
    for (usize i = 0; i < world->static_bodies.capacity; ++i) {
        Phw_Character_Body *body = phw_array_get_at(&world->static_bodies, i);
        body->last_query_id      = 0;
    }
}

static bool collides_grid(
    Phw_World                *world,
    const Phw_Spatial_Grid   *grid,
    Phw_Bbox                  bbox,
    Phw_Collision_Shape       shape,
    const Phw_Character_Body *ignore,
    bool                      precise,
    u32                       layer_mask
)
{
    Phw_Point start, end;
    if (!phw_spatialgrid_get_range(grid, bbox, &start, &end))
        return false;
    for (i32 y = start.y; y <= end.y; ++y) {
        for (i32 x = start.x; x <= end.x; ++x) {
            Phw_Spatial_Grid_Bucket *bucket =
                phw_spatialgrid_get_bucket(grid, (usize)x + (usize)y * (usize)grid->columns);
            for (usize k = 0; k < bucket->count; ++k) {
                Phw_Character_Body *body =
                    *(Phw_Character_Body **)phw_array_get_at(&bucket->bodies_pointers_array, k);
                if (!body || !body->active || !body->enabled || body == ignore ||
                    (layer_mask && !phw_hasAnyFlag(body->user_layer_mask, layer_mask)))
                    continue;
                if (body->last_query_id == world->current_query_id)
                    continue;
                body->last_query_id = world->current_query_id;
                Phw_Bbox bb         = body->bbox;
                bb.position.x       = phw_ffloor(bb.position.x);
                bb.position.y       = phw_ffloor(bb.position.y);
                if (!phw_rect_vs_rect(
                        (Phw_Rect){
                            bbox.position, { bbox.h_size.x * 2, bbox.h_size.y * 2 }
                },
                        (Phw_Rect){ bb.position, { bb.h_size.x * 2, bb.h_size.y * 2 } }
                    ))
                    continue;
                if (!precise || overlaps_shapes(
                                    bbox.position, shape, body->bbox.position, body->collision_shape
                                ))
                    return true;
            }
        }
    }
    return false;
}

bool phw_world_colliding_at(
    Phw_World                *world,
    Phw_Vec2                  position,
    Phw_Collision_Shape       collision_shape,
    bool                      do_precise_check,
    const Phw_Character_Body *ignore_body,
    Phw_Pipeline_Mask         pipeline_mask,
    u32                       user_layer_mask,
    u8                        tile_type,
    u32                       user_tile_mask
)
{
    if (!world || !valid_shape(collision_shape))
        return false;
    if (ignore_body && ignore_body->world != world)
        return false;
    position.x    = phw_ffloor(position.x);
    position.y    = phw_ffloor(position.y);
    Phw_Bbox bbox = { position, half_size(collision_shape) };
    next_query(world);
    if ((pipeline_mask == PHW_PIPE_MASK_ALL ||
         phw_hasAnyFlag(pipeline_mask, PHW_PIPE_MASK_DYNAMIC_OBJECTS)) &&
        collides_grid(
            world,
            &world->spatial_grid,
            bbox,
            collision_shape,
            ignore_body,
            do_precise_check,
            user_layer_mask
        ))
        return true;
    if ((pipeline_mask == PHW_PIPE_MASK_ALL ||
         phw_hasAnyFlag(pipeline_mask, PHW_PIPE_MASK_STATIC_OBJECTS)) &&
        collides_grid(
            world,
            &world->static_grid,
            bbox,
            collision_shape,
            ignore_body,
            do_precise_check,
            user_layer_mask
        ))
        return true;
    if ((pipeline_mask == PHW_PIPE_MASK_ALL ||
         phw_hasAnyFlag(pipeline_mask, PHW_PIPE_MASK_TILES)) &&
        overlaps_tiles(
            world->tilemap, position, collision_shape, do_precise_check, tile_type, user_tile_mask
        ))
        return true;
    return false;
}

static usize collect_grid(
    Phw_World                *world,
    const Phw_Spatial_Grid   *grid,
    Phw_Bbox                  bbox,
    const Phw_Character_Body *buffer[],
    usize                     max_count,
    usize                     count
)
{
    Phw_Point start, end;
    if (!phw_spatialgrid_get_range(grid, bbox, &start, &end))
        return count;
    for (i32 y = start.y; y <= end.y; ++y) {
        for (i32 x = start.x; x <= end.x; ++x) {
            Phw_Spatial_Grid_Bucket *bucket =
                phw_spatialgrid_get_bucket(grid, (usize)x + (usize)y * (usize)grid->columns);
            for (usize k = 0; k < bucket->count; ++k) {
                Phw_Character_Body *body =
                    *(Phw_Character_Body **)phw_array_get_at(&bucket->bodies_pointers_array, k);
                if (!body || !body->active || !body->enabled ||
                    body->last_query_id == world->current_query_id)
                    continue;
                body->last_query_id  = world->current_query_id;
                Phw_Vec2 indexed_pos = { phw_ffloor(body->bbox.position.x),
                                         phw_ffloor(body->bbox.position.y) };
                if (!phw_rect_vs_rect(
                        (Phw_Rect){
                            bbox.position, { bbox.h_size.x * 2.f, bbox.h_size.y * 2.f }
                },
                        (Phw_Rect){ indexed_pos,
                                    { body->bbox.h_size.x * 2.f, body->bbox.h_size.y * 2.f } }
                    ))
                    continue;
                if (count == max_count)
                    return count;
                buffer[count++] = body;
            }
        }
    }
    return count;
}

usize phw_world_query_bodies_with_mask(
    Phw_World                *world,
    Phw_Bbox                  bbox,
    const Phw_Character_Body *buffer[],
    usize                     buffer_size,
    Phw_Pipeline_Mask         pipeline_mask
)
{
    if (!world || !buffer || !buffer_size)
        return 0;
    next_query(world);
    usize count = 0;
    if (pipeline_mask == PHW_PIPE_MASK_ALL ||
        phw_hasAnyFlag(pipeline_mask, PHW_PIPE_MASK_DYNAMIC_OBJECTS))
        count = collect_grid(world, &world->spatial_grid, bbox, buffer, buffer_size, count);
    if (count < buffer_size && (pipeline_mask == PHW_PIPE_MASK_ALL ||
                                phw_hasAnyFlag(pipeline_mask, PHW_PIPE_MASK_STATIC_OBJECTS)))
        count = collect_grid(world, &world->static_grid, bbox, buffer, buffer_size, count);
    return count;
}

usize phw_world_query_bodies(
    Phw_World                *world,
    usize                     internal_buckets_buffer_lockup_size,
    Phw_Bbox                  bbox,
    const Phw_Character_Body *bodies_buffer[],
    usize                     bodies_buffer_length
)
{
    /* Obsolete lookup-size parameter is retained for source compatibility. */
    (void)internal_buckets_buffer_lockup_size;
    return phw_world_query_bodies_with_mask(
        world, bbox, bodies_buffer, bodies_buffer_length, PHW_PIPE_MASK_DYNAMIC_OBJECTS
    );
}

void phw_world_destroy(Phw_World *world)
{
    if (!world)
        return;
    phw_spatialgrid_destroy(&world->spatial_grid);
    phw_spatialgrid_destroy(&world->static_grid);
    phw_array_destroy(&world->bodies);
    phw_array_destroy(&world->static_bodies);
    *world = (Phw_World){ 0 };
}

void phw_world_clear(Phw_World *w)
{
    if (!w) {
        return;
    }

    phw_spatialgrid_clear_buckets(&w->spatial_grid);
    for (usize i = 0; i < w->bodies_count; i++) {
        phw_array_remove_at(&w->bodies, i);
    }
    w->bodies_count = 0;
}
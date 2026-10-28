#include "phw_debug_draw.h"
#include "phw_array.h"
#include "phw_common.h"
#include "phw_spatial_grid.h"
#include "phw_tilemap.h"

Phw_Debug_Draw phw_debug_draw_create(void)
{
    return (Phw_Debug_Draw){ 0 };
}

static void draw_body_pool(const Phw_Debug_Draw *debug, const Phw_Array *pool)
{
    if (!debug->render_bodies || !pool->data) return;
    for (usize i = 0; i < pool->capacity; ++i) {
        const Phw_Character_Body *body = phw_array_get_at(pool, i);
        if (!body->active) continue;
        Phw_Bbox display_bbox = body->bbox;
        display_bbox.position.x = phw_ffloor(display_bbox.position.x);
        display_bbox.position.y = phw_ffloor(display_bbox.position.y);
        Phw_Debug_Draw_Body_Data data = {
            display_bbox, body->collision_shape, body->enabled
        };
        debug->render_bodies(data, debug->user_data);
    }
}

void phw_debug_draw_process_data(
    const Phw_Debug_Draw *debug, const Phw_World *world, const Phw_Rect *tiles_view_bounds
)
{
    if (!debug || !world) return;
    const Phw_Spatial_Grid *grid = &world->spatial_grid;
    if (grid->buckets_array.data && grid->columns > 0 && grid->rows > 0 &&
        (debug->render_buckets_grid || debug->render_buckets_under_bodies)) {
        f32 w = grid->bounds.size.x / grid->columns;
        f32 h = grid->bounds.size.y / grid->rows;
        for (usize i = 0; i < grid->buckets_array.capacity; ++i) {
            i32 x = (i32)(i % (usize)grid->columns);
            i32 y = (i32)(i / (usize)grid->columns);
            Phw_Debug_Draw_Bucket_Data data = {
                { grid->bounds.position.x + x * w, grid->bounds.position.y + y * h },
                { w, h }
            };
            if (debug->render_buckets_grid)
                debug->render_buckets_grid(data, debug->user_data);
            if (debug->render_buckets_under_bodies) {
                Phw_Spatial_Grid_Bucket *dynamic = phw_spatialgrid_get_bucket(grid, i);
                Phw_Spatial_Grid_Bucket *st = phw_spatialgrid_get_bucket(&world->static_grid, i);
                if ((dynamic && dynamic->count) || (st && st->count))
                    debug->render_buckets_under_bodies(data, debug->user_data);
            }
        }
    }

    const Phw_Tilemap *map = world->tilemap;
    if (debug->render_tiles && map && map->tile_size && map->rows > 0 && map->columns > 0) {
        i32 sx = 0, sy = 0, ex = map->columns - 1, ey = map->rows - 1;
        if (tiles_view_bounds) {
            /* Debug view position is the TOP-LEFT corner (legacy convention). */
            sx = (i32)phw_ffloor(tiles_view_bounds->position.x / map->tile_size);
            sy = (i32)phw_ffloor(tiles_view_bounds->position.y / map->tile_size);
            ex = (i32)phw_ffloor((tiles_view_bounds->position.x +
                                 tiles_view_bounds->size.x) / map->tile_size);
            ey = (i32)phw_ffloor((tiles_view_bounds->position.y +
                                 tiles_view_bounds->size.y) / map->tile_size);
            sx = phw_imax(sx, 0); sy = phw_imax(sy, 0);
            ex = phw_imin(ex, map->columns - 1);
            ey = phw_imin(ey, map->rows - 1);
        }
        for (i32 y = sy; y <= ey; ++y) {
            for (i32 x = sx; x <= ex; ++x) {
                Phw_Tile tile = phw_tmap_get_Tile(map, (Phw_Point){ x, y });
                if (tile.type == PHW_TILETYPE_EMPTY) continue;
                Phw_Debug_Draw_Tile_Data data = {
                    { (f32)x * map->tile_size, (f32)y * map->tile_size },
                    (f32)map->tile_size, tile
                };
                debug->render_tiles(data, debug->user_data);
            }
        }
    }

    draw_body_pool(debug, &world->bodies);
    draw_body_pool(debug, &world->static_bodies);
}

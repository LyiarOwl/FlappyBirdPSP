#include "phw_spatial_grid.h"
#include "phw_common.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

static Phw_Point empty_point(void) { return (Phw_Point){ -1, -1 }; }

static bool has_range(const Phw_Character_Body *body)
{
    return body->bucket_start.x >= 0 && body->bucket_start.y >= 0 && body->bucket_end.x >= 0 &&
           body->bucket_end.y >= 0;
}

Phw_Spatial_Grid phw_spatialgrid_create(
    Phw_Rect bounds, i32 rows_divisions, i32 columns_divisions, usize max_capac_per_bucket
)
{
    Phw_Spatial_Grid grid = { 0 };
    if (bounds.size.x <= 0.f || bounds.size.y <= 0.f || rows_divisions <= 0 ||
        columns_divisions <= 0 || max_capac_per_bucket == 0)
        return grid;

    usize rows = (usize)rows_divisions;
    usize cols = (usize)columns_divisions;
    if (cols > SIZE_MAX / rows)
        return grid;
    usize count = cols * rows;
    if (count > SIZE_MAX / max_capac_per_bucket)
        return grid;
    usize slots = count * max_capac_per_bucket;

    grid.buckets_array = phw_array_create(count, sizeof(Phw_Spatial_Grid_Bucket));
    if (!grid.buckets_array.data)
        return grid;
    grid.slots_storage = phw_array_create(slots, sizeof(Phw_Character_Body *));
    if (!grid.slots_storage.data) {
        phw_array_destroy(&grid.buckets_array);
        return (Phw_Spatial_Grid){ 0 };
    }

    for (usize i = 0; i < count; ++i) {
        Phw_Spatial_Grid_Bucket *bucket = phw_array_get_at(&grid.buckets_array, i);
        bucket->bodies_pointers_array.data =
            grid.slots_storage.data + (i * max_capac_per_bucket * sizeof(Phw_Character_Body *));
        bucket->bodies_pointers_array.capacity     = max_capac_per_bucket;
        bucket->bodies_pointers_array.element_size = sizeof(Phw_Character_Body *);
    }
    grid.bounds  = bounds;
    grid.rows    = rows_divisions;
    grid.columns = columns_divisions;
    return grid;
}

bool phw_spatialgrid_get_range(
    const Phw_Spatial_Grid *grid, Phw_Bbox bbox, Phw_Point *start, Phw_Point *end
)
{
    if (!grid || !start || !end || !grid->buckets_array.data || grid->rows <= 0 ||
        grid->columns <= 0 || bbox.h_size.x < 0.f || bbox.h_size.y < 0.f)
        return false;

    const f32 cell_w = grid->bounds.size.x / (f32)grid->columns;
    const f32 cell_h = grid->bounds.size.y / (f32)grid->rows;
    const f32 left   = bbox.position.x - bbox.h_size.x - grid->bounds.position.x;
    const f32 top    = bbox.position.y - bbox.h_size.y - grid->bounds.position.y;
    const f32 right  = bbox.position.x + bbox.h_size.x - grid->bounds.position.x;
    const f32 bottom = bbox.position.y + bbox.h_size.y - grid->bounds.position.y;

    /* Conservative bucket coverage. Narrow-phase excludes boundary-only contact. */
    if (right < 0.f || bottom < 0.f || left >= grid->bounds.size.x || top >= grid->bounds.size.y)
        return false;
    /* Clamp as floating point BEFORE converting to i32 (no out-of-range cast). */
    i32 sx = left <= 0.f ? 0 : (i32)phw_ffloor(left / cell_w);
    i32 sy = top <= 0.f ? 0 : (i32)phw_ffloor(top / cell_h);
    i32 ex = right >= grid->bounds.size.x ? grid->columns - 1
                                          : (right <= 0.f ? 0 : (i32)phw_ffloor(right / cell_w));
    i32 ey = bottom >= grid->bounds.size.y ? grid->rows - 1
                                           : (bottom <= 0.f ? 0 : (i32)phw_ffloor(bottom / cell_h));
    start->x = phw_iclamp(sx, 0, grid->columns - 1);
    start->y = phw_iclamp(sy, 0, grid->rows - 1);
    end->x   = phw_iclamp(ex, 0, grid->columns - 1);
    end->y   = phw_iclamp(ey, 0, grid->rows - 1);
    return true;
}

Phw_Spatial_Grid_Bucket *phw_spatialgrid_get_bucket(const Phw_Spatial_Grid *grid, usize id)
{
    if (!grid || !grid->buckets_array.data || id >= grid->buckets_array.capacity)
        return NULL;
    return phw_array_get_at(&grid->buckets_array, id);
}

/* A failed move leaves every old membership intact. */
bool phw_spatialgrid_push(Phw_Spatial_Grid *grid, Phw_Character_Body *body)
{
    if (!grid || !body || !grid->buckets_array.data)
        return false;
    Phw_Point start = empty_point(), end = empty_point();
    Phw_Bbox  index_bbox  = body->bbox;
    index_bbox.position.x = phw_ffloor(index_bbox.position.x);
    index_bbox.position.y = phw_ffloor(index_bbox.position.y);
    bool in_grid          = phw_spatialgrid_get_range(grid, index_bbox, &start, &end);
    if (in_grid && phw_point_equals(body->bucket_start, start) &&
        phw_point_equals(body->bucket_end, end))
        return true;
    if (!in_grid && !has_range(body))
        return true;

    /* Preflight only NEW cells; an old cell's slot is reusable. */
    if (in_grid) {
        for (i32 y = start.y; y <= end.y; ++y) {
            for (i32 x = start.x; x <= end.x; ++x) {
                bool old = has_range(body) && x >= body->bucket_start.x &&
                           x <= body->bucket_end.x && y >= body->bucket_start.y &&
                           y <= body->bucket_end.y;
                if (old)
                    continue;
                Phw_Spatial_Grid_Bucket *bucket =
                    phw_spatialgrid_get_bucket(grid, (usize)x + (usize)y * (usize)grid->columns);
                if (!bucket || bucket->count >= bucket->bodies_pointers_array.capacity)
                    return false;
            }
        }
    }

    /* Remove old-only cells; retained cells are never duplicated. */
    if (has_range(body)) {
        for (i32 y = body->bucket_start.y; y <= body->bucket_end.y; ++y) {
            for (i32 x = body->bucket_start.x; x <= body->bucket_end.x; ++x) {
                bool retained = in_grid && x >= start.x && x <= end.x && y >= start.y && y <= end.y;
                if (retained)
                    continue;
                Phw_Spatial_Grid_Bucket *bucket =
                    phw_spatialgrid_get_bucket(grid, (usize)x + (usize)y * (usize)grid->columns);
                phw_spatialgrid_bucket_remove(bucket, body);
            }
        }
    }
    if (in_grid) {
        for (i32 y = start.y; y <= end.y; ++y) {
            for (i32 x = start.x; x <= end.x; ++x) {
                bool retained = has_range(body) && x >= body->bucket_start.x &&
                                x <= body->bucket_end.x && y >= body->bucket_start.y &&
                                y <= body->bucket_end.y;
                if (retained)
                    continue;
                Phw_Spatial_Grid_Bucket *bucket =
                    phw_spatialgrid_get_bucket(grid, (usize)x + (usize)y * (usize)grid->columns);
                phw_spatialgrid_bucket_push(bucket, body);
            }
        }
    }
    body->bucket_start = start;
    body->bucket_end   = end;
    return true;
}

void phw_spatialgrid_remove(Phw_Spatial_Grid *grid, Phw_Character_Body *body)
{
    if (!grid || !body || !has_range(body))
        return;
    for (i32 y = body->bucket_start.y; y <= body->bucket_end.y; ++y)
        for (i32 x = body->bucket_start.x; x <= body->bucket_end.x; ++x)
            phw_spatialgrid_bucket_remove(
                phw_spatialgrid_get_bucket(grid, (usize)x + (usize)y * (usize)grid->columns), body
            );
    body->bucket_start = empty_point();
    body->bucket_end   = empty_point();
}

usize phw_spatialgrid_query(
    const Phw_Spatial_Grid  *grid,
    Phw_Bbox                 bbox,
    Phw_Spatial_Grid_Bucket *buffer[],
    usize                    buffer_size
)
{
    if (!buffer || !buffer_size)
        return 0;
    Phw_Point start, end;
    if (!phw_spatialgrid_get_range(grid, bbox, &start, &end))
        return 0;
    usize count = 0;
    for (i32 y = start.y; y <= end.y; ++y)
        for (i32 x = start.x; x <= end.x; ++x) {
            if (count == buffer_size)
                return count;
            buffer[count++] =
                phw_spatialgrid_get_bucket(grid, (usize)x + (usize)y * (usize)grid->columns);
        }
    return count;
}

void phw_spatialgrid_clear_buckets(Phw_Spatial_Grid *grid)
{
    if (!grid)
        return;
    for (usize i = 0; i < grid->buckets_array.capacity; ++i) {
        Phw_Spatial_Grid_Bucket *bucket = phw_spatialgrid_get_bucket(grid, i);
        for (usize j = 0; j < bucket->count; ++j) {
            Phw_Character_Body **slot = phw_array_get_at(&bucket->bodies_pointers_array, j);
            if (*slot) {
                (*slot)->bucket_start = empty_point();
                (*slot)->bucket_end   = empty_point();
            }
        }
        bucket->count = 0;
    }
}

void phw_spatialgrid_destroy(Phw_Spatial_Grid *grid)
{
    if (!grid)
        return;
    /* Slot views belong to slots_storage; do not free them individually. */
    phw_array_destroy(&grid->slots_storage);
    phw_array_destroy(&grid->buckets_array);
    *grid = (Phw_Spatial_Grid){ 0 };
}

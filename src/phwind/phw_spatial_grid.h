#pragma once

#include "phw_rect.h"
#include "phw_array.h"
#include "phw_bbox.h"
#include "phw_character_body.h"
#include "phw_spatial_grid_bucket.h"
#include "phw_types.h"
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct phw_spatial_grid
{
    /* bounds.position is the TOP-LEFT corner of the grid (legacy convention). */
    Phw_Rect bounds;
    i32 columns;
    i32 rows;
    Phw_Array buckets_array;
    /* Single allocation for all bucket slots: avoids per-cell heap overhead. */
    Phw_Array slots_storage;
} Phw_Spatial_Grid;

Phw_Spatial_Grid phw_spatialgrid_create(
    Phw_Rect bounds, i32 rows_divisions, i32 columns_divisions, usize max_capac_per_bucket
);

/* Atomic re-indexing: failure never loses the body's previous bucket entries. */
bool phw_spatialgrid_push(Phw_Spatial_Grid *, Phw_Character_Body *);
void phw_spatialgrid_remove(Phw_Spatial_Grid *, Phw_Character_Body *);

Phw_Spatial_Grid_Bucket *phw_spatialgrid_get_bucket(const Phw_Spatial_Grid *, usize id);

/* Returns the clamped inclusive range; false if it does not overlap the grid. */
bool phw_spatialgrid_get_range(
    const Phw_Spatial_Grid *, Phw_Bbox bbox, Phw_Point *start, Phw_Point *end
);

/* The buffer limits results; use get_range for complete traversal. */
usize phw_spatialgrid_query(
    const Phw_Spatial_Grid *, Phw_Bbox bbox,
    Phw_Spatial_Grid_Bucket *buffer[], usize buffer_size
);

void phw_spatialgrid_clear_buckets(Phw_Spatial_Grid *);
void phw_spatialgrid_destroy(Phw_Spatial_Grid *);

#ifdef __cplusplus
}
#endif

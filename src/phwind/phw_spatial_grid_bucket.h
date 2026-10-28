#pragma once

#include "phw_array.h"
#include "phw_character_body.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Grid-owned buckets point into shared slot storage; never destroy them individually. */
typedef struct phw_spatial_grid_bucket
{
    // usize                     bodies_length;
    // Phw_Character_Body *bodies[]; // array of pointers
    usize     count;
    Phw_Array bodies_pointers_array;
} Phw_Spatial_Grid_Bucket;

Phw_Spatial_Grid_Bucket phw_spatialgrid_bucket_create(usize max_capacity);

void phw_spatialgrid_bucket_push(
    Phw_Spatial_Grid_Bucket *, Phw_Character_Body *
);

void phw_spatialgrid_bucket_remove(
    Phw_Spatial_Grid_Bucket *, Phw_Character_Body *
);

void phw_spatialgrid_bucket_clear(Phw_Spatial_Grid_Bucket *);

void phw_spatialgrid_bucket_destroy(Phw_Spatial_Grid_Bucket *);

// this function does address comparison! o(n)
bool phw_spatialgrid_bucket_contains_body(
    const Phw_Spatial_Grid_Bucket *, Phw_Character_Body *
);

#ifdef __cplusplus
}
#endif

#include "phw_spatial_grid_bucket.h"
#include "phw_array.h"
#include "phw_character_body.h"
#include "phw_common.h"

#include <stdbool.h>
#include <string.h>

Phw_Spatial_Grid_Bucket phw_spatialgrid_bucket_create(usize max_capacity)
{
    Phw_Spatial_Grid_Bucket bckt = { 0 };

    if (max_capacity == 0) {
        phw_assert(
            false,
            "[>>] phw_spatialgrid_bucket_create: max_capacity must be > 0!"
        );
        return bckt;
    }

    bckt.bodies_pointers_array =
        phw_array_create(max_capacity, sizeof(Phw_Character_Body *));

    return bckt;
}

void phw_spatialgrid_bucket_push(
    Phw_Spatial_Grid_Bucket *bucket, Phw_Character_Body *body
)
{
    if (!bucket) {
        phw_assert(
            false,
            "[>] phwSpatialGridBucket_push: spatial grid bucket reference is "
            "null!"
        );
        return;
    }

    if (bucket->count >= bucket->bodies_pointers_array.capacity) {
        phw_assert(
            false, "[>] phwSpatialGridBucket_push: bodies_array overflow!"
        );
        return;
    }

    // [1, 2, 3, 4] => if this happens: [1, NULL, 3, 4], as count will be
    // decreased, then the last item will be overwritten!

    phw_array_set_at(&bucket->bodies_pointers_array, bucket->count, &body);
    bucket->count++;
}

void phw_spatialgrid_bucket_remove(
    Phw_Spatial_Grid_Bucket *bucket, Phw_Character_Body *body
)
{
    if (!bucket || !body)
        return;

    for (usize i = 0; i < bucket->count; i++) {
        Phw_Character_Body **slot =
            phw_array_get_at(&bucket->bodies_pointers_array, i);

        if (*slot != body)
            continue;

        usize last_index = bucket->count - 1;

        if (i != last_index) {
            Phw_Character_Body **last_slot =
                phw_array_get_at(&bucket->bodies_pointers_array, last_index);

            Phw_Character_Body *last_body = *last_slot;

            phw_array_set_at(&bucket->bodies_pointers_array, i, &last_body);
        }

        bucket->count--;
        Phw_Character_Body *null_body = NULL;
        phw_array_set_at(&bucket->bodies_pointers_array, bucket->count, &null_body);
        return;
    }
}

void phw_spatialgrid_bucket_clear(Phw_Spatial_Grid_Bucket *bucket)
{
    if (!bucket) {
        phw_assert(
            false,
            "[>] phwSpatialGridBucket_push: spatial grid bucket reference is "
            "null!"
        );
        return;
    }

    bucket->count = 0;
}

bool phw_spatialgrid_bucket_contains_body(
    const Phw_Spatial_Grid_Bucket *bucket, Phw_Character_Body *body
)
{
    if (!bucket || !body)
        return false;

    for (usize i = 0; i < bucket->count; i++) {
        Phw_Character_Body *const *slot =
            phw_array_get_at(&bucket->bodies_pointers_array, i);

        if (*slot == body)
            return true;
    }

    return false;
}

void phw_spatialgrid_bucket_destroy(Phw_Spatial_Grid_Bucket *bucket)
{
    if (!bucket)
        return;
    phw_array_destroy(&bucket->bodies_pointers_array);
    bucket->count = 0;
}

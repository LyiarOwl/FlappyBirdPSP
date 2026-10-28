#pragma once

#include "phw_bbox.h"
#include "phw_point.h"
#include "phw_tile.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct phw_tilemap
{
    u8       tile_size;
    i32      rows;
    i32      columns;
    usize    tilesLength;
    Phw_Tile tiles[];
} Phw_Tilemap;

/**
 * @param data tiles in bytes (`0`: empty, `1`: solid, `2`: slope up right, `3`:
 * slope up left, `4`: slope down right, `5`: slope down left).
 */
Phw_Tilemap *
phw_tmap_create(i32 rows, i32 columns, u8 tileSize, const u8 data[]);

Phw_Tilemap *phw_tmap_create_from_tiles(
    i32 rows, i32 columns, uint8_t tileSize, const Phw_Tile data[]
);

void phw_tmap_destroy(Phw_Tilemap *);

Phw_Tile phw_tmap_get_Tile(const Phw_Tilemap *, Phw_Point index);

void phw_tmap_set_tile(Phw_Tilemap *, Phw_Point index, Phw_Tile);

usize phw_tmap_query_tiles(
    Phw_Tilemap *,
    Phw_Bbox       bbox,
    u8             tile_type,
    u32            tile_mask,
    Phw_Query_Tile tiles_buffer[],
    usize          tiles_buffer_length
);

#ifdef __cplusplus
}
#endif

#include "phw_bbox.h"
#include "phw_common.h"
#include "phw_point.h"
#include "phw_tile.h"
#include "phw_tilemap.h"
#include "phw_vec2.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static Phw_Tile readTileFromByte(uint8_t byte)
{
    switch (byte) {

    case 0: // empty
        return (Phw_Tile){ .type = PHW_TILETYPE_EMPTY, .angle = 0 };
    case 1: // solid
        return (Phw_Tile){ .type = PHW_TILETYPE_SOLID, .angle = 0 };
    case 2: // slope (up right)
        return (Phw_Tile){ .type  = PHW_TILETYPE_SLOPE,
                           .angle = PHW_SLOPE_ANGLE_UP_RIGHT };
    case 3: // slope (up left)
        return (Phw_Tile){ .type  = PHW_TILETYPE_SLOPE,
                           .angle = PHW_SLOPE_ANGLE_UP_LEFT };
    case 4: // slope (down right)
        return (Phw_Tile){ .type  = PHW_TILETYPE_SLOPE,
                           .angle = PHW_SLOPE_ANGLE_DOWN_RIGHT };
    case 5: // slope (down left)
        return (Phw_Tile){ .type  = PHW_TILETYPE_SLOPE,
                           .angle = PHW_SLOPE_ANGLE_DOWN_LEFT };
    default:
        return (Phw_Tile){ 0 };
    }
}

Phw_Tilemap *
phw_tmap_create(i32 rows, i32 columns, u8 tileSize, const u8 data[])
{
    if (!data || rows <= 0 || columns <= 0 || tileSize == 0) {
        phw_assert(false, "[>] phwTmap_create: data is null!");
        return NULL;
    }
    if ((size_t)rows > SIZE_MAX / (size_t)columns)
        return NULL;
    size_t length = (size_t)rows * (size_t)columns;
    if (length > (SIZE_MAX - sizeof(Phw_Tilemap)) / sizeof(Phw_Tile))
        return NULL;

    Phw_Tilemap *tmap = (Phw_Tilemap *)calloc(
        1, sizeof(Phw_Tilemap) + length * sizeof(Phw_Tile)
    );
    if (!tmap) return NULL;

    tmap->tilesLength = length;
    tmap->columns     = columns;
    tmap->rows        = rows;
    tmap->tile_size   = tileSize;

    for (size_t i = 0; i < length; i++) {
        tmap->tiles[i] = readTileFromByte(data[i]);
    }

    return tmap;
}

Phw_Tilemap *phw_tmap_create_from_tiles(
    i32 rows, i32 columns, uint8_t tileSize, const Phw_Tile data[]
)
{
    if (!data || rows <= 0 || columns <= 0 || tileSize == 0) {
        phw_assert(false, "[>] phwTmap_createFromTiles: data is null!");
        return NULL;
    }
    if ((size_t)rows > SIZE_MAX / (size_t)columns)
        return NULL;
    size_t length = (size_t)rows * (size_t)columns;
    if (length > (SIZE_MAX - sizeof(Phw_Tilemap)) / sizeof(Phw_Tile))
        return NULL;

    Phw_Tilemap *tmap = (Phw_Tilemap *)calloc(
        1, sizeof(Phw_Tilemap) + length * sizeof(Phw_Tile)
    );
    if (!tmap) return NULL;

    tmap->tilesLength = length;
    tmap->columns     = columns;
    tmap->rows        = rows;
    tmap->tile_size   = tileSize;

    memcpy(tmap->tiles, data, tmap->tilesLength * sizeof(Phw_Tile));

    return tmap;
}

void phw_tmap_destroy(Phw_Tilemap *tmap)
{
    if (!tmap)
        return;
    free(tmap);
}

Phw_Tile phw_tmap_get_Tile(const Phw_Tilemap *tmap, Phw_Point index)
{
    if (!tmap) {
        phw_assert(false, "[>] phwTmap_getTile: tilemap reference is null!");
        return (Phw_Tile){ 0 };
    }
    if (tmap->tilesLength == 0)
        return (Phw_Tile){ 0 };
    if (index.x < 0 || index.x >= tmap->columns)
        return (Phw_Tile){ 0 };
    if (index.y < 0 || index.y >= tmap->rows)
        return (Phw_Tile){ 0 };

    return tmap->tiles[(usize)index.x + (usize)index.y * (usize)tmap->columns];
}

void phw_tmap_set_tile(Phw_Tilemap *tmap, Phw_Point index, Phw_Tile tile)
{
    if (!tmap) {
        phw_assert(false, "[>] phwTmap_setTile: tilemap reference is null!");
        return;
    }
    if (tmap->tilesLength == 0)
        return;
    if (index.x < 0 || index.x >= tmap->columns)
        return;
    if (index.y < 0 || index.y >= tmap->rows)
        return;

    tmap->tiles[(usize)index.x + (usize)index.y * (usize)tmap->columns] = tile;
}

usize phw_tmap_query_tiles(
    Phw_Tilemap   *tmap,
    Phw_Bbox       bbox,
    u8             tile_type,
    u32            tile_mask,
    Phw_Query_Tile tiles_buffer[],
    usize          tiles_buffer_length
)
{
    if (!tmap) {
        phw_assert(false, "[>] phwTmap_querytiles: tilemap reference is null!");
        return (size_t)0;
    }

    if (!tiles_buffer || tiles_buffer_length == 0) {
        phw_assert(false, "[>] phwTmap_querytiles: buffer reference is null!");
        return (size_t)0;
    }

    if (!tmap->tile_size) return 0;

    int startX = (int)phw_ffloor(phw_bbox_get_left(&bbox) / tmap->tile_size);
    int startY = (int)phw_ffloor(phw_bbox_get_top(&bbox) / tmap->tile_size);
    /* Conservative inclusive coverage: -1 would lose sub-pixel overlap. */
    int endX = (int)phw_ffloor(phw_bbox_get_right(&bbox) / tmap->tile_size);
    int endY = (int)phw_ffloor(phw_bbox_get_bottom(&bbox) / tmap->tile_size);

    startX = phw_imax(0, startX);
    startY = phw_imax(0, startY);
    endX   = phw_imin(tmap->columns - 1, endX);
    endY   = phw_imin(tmap->rows - 1, endY);

    size_t index = 0;
    for (int x = startX; x <= endX; x++) {
        for (int y = startY; y <= endY; y++) {
            const Phw_Tile tile = phw_tmap_get_Tile(tmap, (Phw_Point){ x, y });
            if (tile.type == PHW_TILETYPE_EMPTY)
                continue;

            bool matchesType = tile_type == 0 ||
                               phw_hasAnyFlag((u32)tile.type, (u32)tile_type);
            if (!matchesType)
                continue;

            bool matchesMask =
                tile_mask == 0 ||
                phw_hasAnyFlag((u32)tile.user_tile_mask, (u32)tile_mask);
            if (!matchesMask)
                continue;

            if (index >= tiles_buffer_length)
                return index;

            // clang-format off
            tiles_buffer[index++] = (Phw_Query_Tile){
                (Phw_Point){
                    x, y
                },
                (Phw_Vec2){
                    x * tmap->tile_size,
                    y * tmap->tile_size
                },
                tmap->tile_size,
                tile
            };
            // clang-format on
        }
    }
    return index;
}

#pragma once

// #include <stdint.h>
#include "phw_vec2.h"
#include "phw_types.h"
#include "phw_point.h"

// typedef uint8_t Phw_Tile_Type;
// typedef uint8_t PhwSlopeAngle;
// typedef uint8_t PhwUserTileMask;

typedef enum phw_tile_type {
    PHW_TILETYPE_EMPTY = 0,
    PHW_TILETYPE_SOLID = 1 << 0,
    PHW_TILETYPE_SLOPE = 1 << 1
} Phw_Tile_Type;

typedef enum phw_slope_angle {
    PHW_SLOPE_ANGLE_NONE       = 0,
    PHW_SLOPE_ANGLE_UP_RIGHT   = 1,
    PHW_SLOPE_ANGLE_UP_LEFT    = 2,
    PHW_SLOPE_ANGLE_DOWN_RIGHT = 3,
    PHW_SLOPE_ANGLE_DOWN_LEFT  = 4
} Phw_Slope_Angle;

typedef struct phw_tile
{
    u8  type;
    u8  angle;
    u32 user_tile_mask;
} Phw_Tile;

typedef struct phw_query_tile
{
    Phw_Point index;
    Phw_Vec2  position;
    float     tile_size;
    Phw_Tile  tile;
} Phw_Query_Tile;

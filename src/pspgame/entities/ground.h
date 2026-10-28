#pragma once

#include "lframework/texture_region.h"
#include "lframework/vec2.h"

typedef struct ground
{
    Texture_Region region;
    Vec2           position;
    f32            scroll_speed;
} Ground;

Ground new_ground(Texture2D *);

void update_ground(Ground *);

void draw_ground(Ground *);

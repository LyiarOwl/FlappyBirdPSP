#pragma once

#include "lframework/texture_region.h"
#include "lframework/vec2.h"

typedef struct background
{
    Texture_Region region;
    Vec2           position;
    f32            scroll_speed;
} Background;

Background create_background(Texture2D *);

void update_background(Background *);

void draw_background(Background *);

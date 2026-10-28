#pragma once

#include "lframework/array.h"
#include "lframework/camera2d.h"
#include "lframework/texture2d.h"
#include "lframework/types.h"
#include "lframework/vec2.h"
#include "phwind/phw_character_body.h"

typedef struct grounds_controller
{
    Array grounds;
    Vec2  offset;

    Phw_Character_Body *body;
} Grounds_Controller;

Grounds_Controller
new_grounds_controller(Texture2D *, usize count, Camera2D *, f32 scroll_spd, Vec2 offset);

void update_grounds_controller(Grounds_Controller *);

void draw_grounds_controller(Grounds_Controller *);

void reset_grounds_controller(Grounds_Controller *);

void free_grounds_controller(Grounds_Controller*);

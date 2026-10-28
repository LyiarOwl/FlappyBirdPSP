#pragma once

#include "lframework/array.h"
#include "lframework/camera2d.h"
#include "lframework/texture2d.h"
#include "lframework/types.h"

typedef struct backgrounds_controller
{
    Array backgrounds;
} Backgrounds_Controller;

Backgrounds_Controller new_backgrounds_controller(
    Texture2D *, usize count, Camera2D *, f32 scroll_spd
);

void update_backgrounds_controller(Backgrounds_Controller *);

void draw_backgrounds_controller(Backgrounds_Controller *);

void reset_backgrounds_controller(Backgrounds_Controller *);

void free_backgrounds_controller(Backgrounds_Controller*);

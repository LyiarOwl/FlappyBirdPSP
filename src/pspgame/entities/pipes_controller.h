#pragma once

#include "lframework/array.h"
#include "lframework/camera2d.h"
#include "lframework/texture2d.h"
#include "pipe.h"
#include "pipes_set.h"

typedef struct pipes_pool_item
{
    bool      active;
    Pipes_Set set;
} Pipes_Pool_Item;

typedef struct pipes_controller
{
    Array pipes_pool_array;
    f32   elapsed;
    f32   spawn_interval;
    f32   scroll_speed;
    Vec2  initial_pos;
    bool  pending_to_spawn;
} Pipes_Controller;

Pipes_Controller new_pipes_controller(
    Texture2D *,
    f32   spawn_interval,
    usize pipes_limit,
    Vec2  initial_pos,
    const Camera2D *,
    void *screen
);

void update_pipes_controller(Pipes_Controller *);

void draw_pipes_controller(Pipes_Controller *);

void reset_pipes_controller(Pipes_Controller *);

void free_pipes_controller(Pipes_Controller*);

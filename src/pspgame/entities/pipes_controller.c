#include "pipes_controller.h"
#include "lframework/array.h"
#include "lframework/camera2d.h"
#include "lframework/common.h"
#include "pipes_set.h"

#include <stdlib.h>

static f32              delta       = 1.f / 60.f;
static bool             first_spawn = false;
static Pipes_Pool_Item *curr_pipe   = NULL;
static Vec2             screen_min;

static float min_y_spawn = 50.f;
static float max_y_spawn = 150.f;

static inline i32 get_random(i32 min, i32 max) { return rand() % (max - min + 1) + min; }

Pipes_Controller new_pipes_controller(
    Texture2D      *texture,
    f32             spawn_interval,
    usize           pipes_limit,
    Vec2            initial_pos,
    const Camera2D *camera,
    void           *screen
)
{
    Pipes_Controller controller = { 0 };
    controller.spawn_interval   = spawn_interval;
    controller.initial_pos      = initial_pos;
    controller.scroll_speed     = 1.f;

    controller.initial_pos.x += 16.f;

    controller.pipes_pool_array = array_new(pipes_limit, sizeof(Pipes_Pool_Item));

    for (usize i = 0; i < controller.pipes_pool_array.capacity; i++) {
        Pipes_Pool_Item item = { 0 };

        item.set = new_pipes_set(texture, screen);

        array_set(&controller.pipes_pool_array, i, &item);
    }

    screen_min = camera2d_screen_to_world(camera, (Vec2){ 0 });

    return controller;
}

static void try_to_get_pipe_from_pool(Pipes_Controller *controller)
{
    if (!controller->pending_to_spawn) {
        for (usize i = 0; i < controller->pipes_pool_array.capacity; i++) {
            Pipes_Pool_Item *tmp = array_get(&controller->pipes_pool_array, i);
            if (!tmp->active) {
                curr_pipe = tmp;

                controller->pending_to_spawn = true;
                break;
            }
        }
    }
}

static void try_to_spawn_pipe(Pipes_Controller *controller)
{
    if (curr_pipe) {
        if (controller->pending_to_spawn) {
            controller->elapsed += delta;
            if (controller->elapsed >= controller->spawn_interval) {
                controller->elapsed -= controller->spawn_interval;
                controller->pending_to_spawn = false;

                curr_pipe->active = true;
                set_pipes_set_position(
                    &curr_pipe->set,
                    (Vec2){ controller->initial_pos.x, (f32)get_random(min_y_spawn, max_y_spawn) }
                );
                set_enabled_pipes_set(&curr_pipe->set, true);
            }
        }
    }
}

static void try_to_update_pipes(Pipes_Controller *controller)
{
    for (usize i = 0; i < controller->pipes_pool_array.capacity; i++) {
        Pipes_Pool_Item *tmp = array_get(&controller->pipes_pool_array, i);
        if (tmp->active) {
            Vec2 pos = get_pipes_set_position(&tmp->set);
            pos.x -= controller->scroll_speed;
            set_pipes_set_position(&tmp->set, pos);
        }
    }
}

static void try_to_disable_pipes_out_of_bounds(Pipes_Controller *controller)
{
    for (usize i = 0; i < controller->pipes_pool_array.capacity; i++) {
        Pipes_Pool_Item *tmp = array_get(&controller->pipes_pool_array, i);
        if (tmp->active) {
            Vec2 pos = get_pipes_set_position(&tmp->set);
            if (pos.x < screen_min.x - 16.f) {
                tmp->active = false;
                set_enabled_pipes_set(&tmp->set, false);
            }
        }
    }
}

void update_pipes_controller(Pipes_Controller *controller)
{
    try_to_get_pipe_from_pool(controller);

    try_to_spawn_pipe(controller);

    try_to_update_pipes(controller);

    try_to_disable_pipes_out_of_bounds(controller);
}

void draw_pipes_controller(Pipes_Controller *controller)
{
    for (usize i = 0; i < controller->pipes_pool_array.capacity; i++) {
        Pipes_Pool_Item *tmp = array_get(&controller->pipes_pool_array, i);
        if (tmp->active) {
            draw_pipes_set(&tmp->set);
        }
    }
}

void reset_pipes_controller(Pipes_Controller *controller)
{
    for (usize i = 0; i < controller->pipes_pool_array.capacity; i++) {
        Pipes_Pool_Item *tmp = array_get(&controller->pipes_pool_array, i);

        tmp->active = false;
        set_enabled_pipes_set(&tmp->set, false);
    }
}

void free_pipes_controller(Pipes_Controller *p)
{
    if (!p)
        return;

    array_free(&p->pipes_pool_array);
}

#include "grounds_controller.h"
#include "ground.h"
#include "layers_mask.h"
#include "lframework/array.h"
#include "lframework/camera2d.h"
#include "phwind/phw_character_body.h"
#include "phwind/phw_shapes.h"
#include "phwind/phw_world.h"
#include "pspgame/globals.h"
#include "settings.h"

#include <string.h>

static void
create_grounds(Grounds_Controller *controller, Texture2D *texture, f32 scroll_spd, Vec2 offset)
{
    for (usize i = 0; i < controller->grounds.capacity; i++) {
        Ground bg          = new_ground(texture);
        controller->offset = offset;
        bg.position.x      = controller->offset.x + i * (bg.region.clip_rect.w - 1);
        bg.position.y      = controller->offset.y;
        bg.scroll_speed    = scroll_spd;
        array_set(&controller->grounds, i, &bg);
    }
}

static Vec2 screen_min;

Grounds_Controller new_grounds_controller(
    Texture2D *texture, usize count, Camera2D *camera, f32 scroll_spd, Vec2 offset
)
{
    Grounds_Controller c = { 0 };

    c.grounds = array_new(count, sizeof(Ground));

    screen_min = camera2d_screen_to_world(camera, (Vec2){ 0 });

    create_grounds(&c, texture, scroll_spd, offset);

    Vec2 screen_max = camera2d_screen_to_world(camera, (Vec2){ SCREEN_WIDTH, SCREEN_HEIGHT });

    f32 view_width  = screen_max.x - screen_min.x;
    f32 view_height = screen_max.y - screen_min.y;

    Vec2 screen_center =
        camera2d_screen_to_world(camera, (Vec2){ SCREEN_WIDTH / 2.f, SCREEN_HEIGHT / 2.f });

    c.body = phw_world_create_character_body(
        &world, phw_create_rect_coll_shape((Phw_Vec2){ view_width / 2.f, 20.f / 2.f })
    );
    c.body->user_layer_mask = LAYER_MASK_GROUND;
    float shape_height      = 0.f;

    if (c.body->collision_shape.type == PHW_COLL_SHAPE_RECT)
        shape_height = c.body->collision_shape.shape.rect.h_size.y;

    phw_body_set_position(
        c.body, (Phw_Vec2){ c.offset.x + view_width / 2.f, c.offset.y + shape_height }
    );

    return c;
}

void update_grounds_controller(Grounds_Controller *controller)
{
    Ground *first = array_get(&controller->grounds, 0);
    Ground *last  = array_get(&controller->grounds, controller->grounds.capacity - 1);

    if (first && last) {
        f32 first_right = first->position.x + first->region.clip_rect.w;

        if (first_right <= screen_min.x) {
            Ground tmp = *first;

            tmp.position.x = last->position.x + last->region.clip_rect.w - 1.f;

            memmove(
                controller->grounds.bytes,
                controller->grounds.bytes + controller->grounds.element_size,
                controller->grounds.element_size * (controller->grounds.capacity - 1)
            );
            array_set(&controller->grounds, controller->grounds.capacity - 1, &tmp);
        }

        for (usize i = 0; i < controller->grounds.capacity; i++) {
            Ground *ground = array_get(&controller->grounds, i);
            if (ground)
                update_ground(ground);
        }
    }
}

void draw_grounds_controller(Grounds_Controller *controller)
{
    for (usize i = 0; i < controller->grounds.capacity; i++) {
        Ground *bg = array_get(&controller->grounds, i);
        if (bg)
            draw_ground(bg);
    }
}

void reset_grounds_controller(Grounds_Controller *controller)
{
    for (usize i = 0; i < controller->grounds.capacity; i++) {
        Ground *ground     = array_get(&controller->grounds, i);
        ground->position.x = controller->offset.x + ground->region.clip_rect.w * i;
        ground->position.y = controller->offset.y;
    }
}

void free_grounds_controller(Grounds_Controller *g)
{
    if (!g)
        return;

    array_free(&g->grounds);
}

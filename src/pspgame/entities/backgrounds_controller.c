#include "backgrounds_controller.h"
#include "background.h"
#include "lframework/array.h"

#include <string.h>

static void create_bgs(Backgrounds_Controller *controller, Texture2D *texture, f32 scroll_spd)
{
    for (usize i = 0; i < controller->backgrounds.capacity; i++) {
        Background bg   = create_background(texture);
        bg.position.x   = i * (bg.region.clip_rect.w - 1);
        bg.scroll_speed = scroll_spd;
        array_set(&controller->backgrounds, i, &bg);
    }
}

static Vec2 screen_min;

Backgrounds_Controller
new_backgrounds_controller(Texture2D *texture, usize count, Camera2D *camera, f32 scroll_spd)
{
    Backgrounds_Controller controller = { 0 };

    controller.backgrounds = array_new(count, sizeof(Background));

    screen_min = camera2d_screen_to_world(camera, (Vec2){ 0 });

    create_bgs(&controller, texture, scroll_spd);

    return controller;
}

void update_backgrounds_controller(Backgrounds_Controller *controller)
{
    Background *first_bg = array_get(&controller->backgrounds, 0);
    Background *last_bg = array_get(&controller->backgrounds, controller->backgrounds.capacity - 1);

    if (first_bg && last_bg) {
        f32 first_bg_right = first_bg->position.x + first_bg->region.clip_rect.w;

        if (first_bg_right <= screen_min.x) {
            Background tmp = *first_bg;

            tmp.position.x = last_bg->position.x + last_bg->region.clip_rect.w - 1.f;

            memmove(
                controller->backgrounds.bytes,
                controller->backgrounds.bytes + controller->backgrounds.element_size,
                controller->backgrounds.element_size * (controller->backgrounds.capacity - 1)
            );
            array_set(&controller->backgrounds, controller->backgrounds.capacity - 1, &tmp);
        }

        for (usize i = 0; i < controller->backgrounds.capacity; i++) {
            Background *bg = array_get(&controller->backgrounds, i);
            if (bg)
                update_background(bg);
        }
    }
}

void draw_backgrounds_controller(Backgrounds_Controller *controller)
{
    for (usize i = 0; i < controller->backgrounds.capacity; i++) {
        Background *bg = array_get(&controller->backgrounds, i);
        if (bg)
            draw_background(bg);
    }
}

void reset_backgrounds_controller(Backgrounds_Controller *controller)
{
    for (usize i = 0; i < controller->backgrounds.capacity; i++) {
        Background *bg = array_get(&controller->backgrounds, i);
        bg->position.x = bg->region.clip_rect.w * i;
    }
}

void free_backgrounds_controller(Backgrounds_Controller *c)
{
    if (!c)
        return;

    array_free(&c->backgrounds);
}

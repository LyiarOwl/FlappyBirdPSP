#include "bird.h"
#include "layers_mask.h"
#include "lframework/animation.h"
#include "pspgame/globals.h"
#include "pspgame/observers_list.h"

#include "lframework/common.h"
#include "lframework/input.h"
#include "lframework/sprite.h"
#include "lframework/texture_region.h"

#include "phwind/phw_character_body.h"
#include "phwind/phw_shapes.h"
#include "phwind/phw_vec2.h"
#include "phwind/phw_world.h"
#include "pspctrl.h"
#include "pspgame/screens/game_screen.h"

static bool check_pipes_collision(Bird *bird)
{
    Phw_Vec2 pos = phw_body_get_position(bird->body);

    return phw_world_colliding_at(
        &world,
        pos,
        bird->body->collision_shape,
        true,
        bird->body,
        PHW_PIPE_MASK_DYNAMIC_OBJECTS,
        LAYER_MASK_PIPE,
        0,
        0
    );
}

static bool check_ground_collision(Bird *bird, Phw_Vec2 vel)
{
    Phw_Vec2 pos = phw_body_get_position(bird->body);
    return phw_world_colliding_at(
        &world,
        (Phw_Vec2){ pos.x + vel.x, pos.y + vel.y },
        bird->body->collision_shape,
        false,
        bird->body,
        PHW_PIPE_MASK_DYNAMIC_OBJECTS,
        LAYER_MASK_GROUND,
        0,
        0
    );
}

static void handle_entering_score_region(Bird *bird)
{
    Phw_Vec2 pos       = phw_body_get_position(bird->body);
    bool     collision = phw_world_colliding_at(
        &world,
        pos,
        bird->body->collision_shape,
        false,
        bird->body,
        PHW_PIPE_MASK_DYNAMIC_OBJECTS,
        LAYER_MASK_SCORE,
        0,
        0
    );

    if (collision) {
        if (!bird->entered_score_collision) {
            observers_list_notify_all(&bird->score_region_collision_observers, NULL);
        }
    }

    bird->entered_score_collision = collision;
}

static bool handle_ground_collision(Bird *bird)
{
    bool collision = false;
    if (check_ground_collision(bird, (Phw_Vec2){ 0.f, bird->vel.y })) {
        collision = true;
        while (!check_ground_collision(bird, (Phw_Vec2){ 0.f, f32_sign(bird->vel.y) })) {
            Phw_Vec2 pos = phw_body_get_position(bird->body);
            pos.y += f32_sign(bird->vel.y);
            phw_body_set_position(bird->body, pos);
        }
        bird->vel.y = 0.f;
    }
    return collision;
}

static f32 delta = 1.f / 60.f;

Bird new_bird(Texture2D *texture, float x, float y, void *screen)
{
    Bird bird = { 0 };

    bird.screen         = screen;
    bird.grav           = 0.15f;
    bird.max_vert_speed = 5.f;
    bird.jump_impulse   = 2.f;

    Texture_Region frames[3] = { texture_region_create(texture, 423, 9, 19, 14),
                                 texture_region_create(texture, 455, 9, 19, 14),
                                 texture_region_create(texture, 487, 9, 19, 14) };

    bird.fly_anim = animation_new(0.1f, frames, 3);
    animation_set_play_mode(&bird.fly_anim, ANIM_PLAY_MODE_NORMAL);

    bird.sprite = sprite_new(frames[0]);
    spr_center_origin(&bird.sprite);

    bird.body = phw_world_create_character_body(&world, phw_create_circle_coll_shape(4.f));
    bird.body->user_layer_mask = LAYER_MASK_BIRD;

    phw_body_set_position(bird.body, (Phw_Vec2){ x, y });
    bird.initial_position = (Phw_Vec2){ x, y };

    bird.score_region_collision_observers = observers_list_new(2);

    return bird;
}

void update_bird(Bird *bird)
{
    bool is_on_floor = check_ground_collision(bird, (Phw_Vec2){ 0.f, 1.f });
    if (!is_on_floor) {
        bird->vel.y = f32_min(bird->vel.y + bird->grav, bird->max_vert_speed);
    }

    Game_Screen *g = bird->screen;
    if (!g->game_over) {
        if (input_was_btn_released(&input, PSP_CTRL_CROSS)) {
            bird->vel.y             = -bird->jump_impulse;
            bird->animation_elapsed = 0.f;
        }
    }

    handle_entering_score_region(bird);

    if (handle_ground_collision(bird)) {
        g->game_over = true;
    }

    if (check_pipes_collision(bird)) {
        g->game_over = true;
    }

    Phw_Vec2 pos = phw_body_get_position(bird->body);
    if (pos.y < 0) {
        g->game_over = true;
    }

    pos.y += bird->vel.y;
    phw_body_set_position(bird->body, pos);
}

void draw_bird(Bird *bird)
{
    Phw_Vec2 pos   = get_bird_position(bird);
    bird->sprite.x = pos.x;
    bird->sprite.y = pos.y;

    Game_Screen *g = bird->screen;
    if (!g->start) {
        bird->animation_elapsed += delta * 0.5f;
        bird->sprite.region =
            *animation_get_keyframe_looping(&bird->fly_anim, bird->animation_elapsed, true);
    } else if (!g->game_over) {
        bird->animation_elapsed += delta;
        bird->sprite.region = *animation_get_keyframe(&bird->fly_anim, bird->animation_elapsed);
    }

    sprite_draw(&bird->sprite, sprite_batch);
}

void free_bird(Bird *bird)
{
    if (!bird)
        return;
    observers_list_free(&bird->score_region_collision_observers);

    animation_free(&bird->fly_anim);
}

void set_bird_position(Bird *bird, float x, float y)
{
    phw_body_set_position(bird->body, (Phw_Vec2){ x, y });
}

Phw_Vec2 get_bird_position(const Bird *bird) { return phw_body_get_position(bird->body); }

void reset_bird(Bird *bird)
{
    bird->vel = (Phw_Vec2){ 0 };
    set_bird_position(bird, bird->initial_position.x, bird->initial_position.y);
}

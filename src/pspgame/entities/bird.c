#include "bird.h"
#include "SDL3_mixer/SDL_mixer.h"
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

    bool collision = phw_world_colliding_at(
        &g_world,
        pos,
        bird->body->collision_shape,
        true,
        bird->body,
        PHW_PIPE_MASK_DYNAMIC_OBJECTS,
        LAYER_MASK_PIPE,
        0,
        0
    );

    if (collision) {
        if (!bird->entered_pipe_collision) {
            MIX_StopTrack(bird->hit_sfx.track, 0);
            MIX_PlayTrack(bird->hit_sfx.track, 0);
            MIX_StopTrack(bird->die_sfx.track, 0);
            MIX_PlayTrack(bird->die_sfx.track, 0);

            bird->entered_pipe_collision = true;
        }
    }
    // bird->entered_pipe_collision = collision;

    return collision;
}

static bool check_ground_collision(Bird *bird, Phw_Vec2 vel)
{
    Phw_Vec2 pos       = phw_body_get_position(bird->body);
    bool     collision = phw_world_colliding_at(
        &g_world,
        (Phw_Vec2){ pos.x + vel.x, pos.y + vel.y },
        bird->body->collision_shape,
        false,
        bird->body,
        PHW_PIPE_MASK_DYNAMIC_OBJECTS,
        LAYER_MASK_GROUND,
        0,
        0
    );

    if (collision) {
        if (!bird->entered_ground_collision && !bird->entered_pipe_collision) {
            bird->entered_ground_collision = true;
            MIX_StopTrack(bird->hit_sfx.track, 0);
            MIX_PlayTrack(bird->hit_sfx.track, 0);
        }
    }
    return collision;
}

static void handle_entering_score_region(Bird *bird)
{
    Phw_Vec2 pos       = phw_body_get_position(bird->body);
    bool     collision = phw_world_colliding_at(
        &g_world,
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
            observers_list_notify_all(&bird->score_region_collision_observers, bird->screen);
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
    bird.grav           = 0.2f;
    bird.max_vert_speed = 4.f;
    bird.jump_impulse   = 2.5f;

    Texture_Region frames[4] = {
        texture_region_create(texture, 455, 9, 19, 14), // 3
        texture_region_create(texture, 487, 9, 19, 14), // 0
        texture_region_create(texture, 455, 9, 19, 14), // 1
        texture_region_create(texture, 423, 9, 19, 14), // 2
    };

    bird.fly_anim = animation_new(0.1f, frames, 4);
    animation_set_play_mode(&bird.fly_anim, ANIM_PLAY_MODE_NORMAL);

    bird.sprite = sprite_new(frames[0]);
    spr_center_origin(&bird.sprite);

    bird.body = phw_world_create_character_body(&g_world, phw_create_circle_coll_shape(6.f));
    bird.body->user_layer_mask = LAYER_MASK_BIRD;

    phw_body_set_position(bird.body, (Phw_Vec2){ x, y });
    bird.initial_position = (Phw_Vec2){ x, y };

    bird.score_region_collision_observers = observers_list_new(1);

    bird.wing_sfx = sound_new(g_mixer, "assets/sounds/sfx_wing.mp3");
    bird.hit_sfx  = sound_new(g_mixer, "assets/sounds/sfx_hit.mp3");
    bird.die_sfx  = sound_new(g_mixer, "assets/sounds/sfx_die.mp3");

    return bird;
}

void update_bird(Bird *bird, bool allow_jump)
{
    bool is_on_floor = check_ground_collision(bird, (Phw_Vec2){ 0.f, 1.f });
    if (!is_on_floor) {
        bird->vel.y = f32_min(bird->vel.y + bird->grav, bird->max_vert_speed);
    }

    if (allow_jump) {
        if (input_was_btn_released(&g_input, PSP_CTRL_CROSS)) {
            bird->vel.y             = -bird->jump_impulse;
            bird->animation_elapsed = 0.f;
            MIX_StopTrack(bird->wing_sfx.track, 0);
            MIX_PlayTrack(bird->wing_sfx.track, 0);
        }
    }

    handle_entering_score_region(bird);

    if (handle_ground_collision(bird)) {
        // g->game_over = true;
        game_screen_switch_to_game_over(bird->screen);
    }

    if (check_pipes_collision(bird)) {
        // g->game_over = true;
        game_screen_switch_to_game_over(bird->screen);
    }

    Phw_Vec2 pos = phw_body_get_position(bird->body);
    if (pos.y < 0) {
        // g->game_over = true;
        game_screen_switch_to_game_over(bird->screen);
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
        bird->animation_elapsed += delta;
        bird->sprite.region =
            *animation_get_keyframe_looping(&bird->fly_anim, bird->animation_elapsed, true);
    } else {
        bird->animation_elapsed += delta;
        bird->sprite.region = *animation_get_keyframe(&bird->fly_anim, bird->animation_elapsed);
    }

    sprite_draw(&bird->sprite, g_sprite_batch);
}

void free_bird(Bird *bird)
{
    if (!bird)
        return;

    sound_free(&bird->wing_sfx);
    sound_free(&bird->hit_sfx);
    sound_free(&bird->die_sfx);

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
    bird->entered_ground_collision = false;
    bird->entered_pipe_collision   = false;
}

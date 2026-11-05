#pragma once

#include "SDL3_mixer/SDL_mixer.h"
#include "lframework/animation.h"
#include "lframework/sprite.h"
#include "pspgame/observers_list.h"

#include "phwind/phw_character_body.h"
#include "phwind/phw_vec2.h"
#include "pspgame/sound.h"

typedef void (*on_bird_score_region_collision_cb)(void);

typedef struct bird
{
    void *screen;

    Phw_Vec2 initial_position;
    Phw_Vec2 vel;
    f32      grav;
    f32      max_vert_speed;
    f32      jump_impulse;
    f32      animation_elapsed;

    bool  entered_score_collision;
    bool  entered_pipe_collision;
    bool  entered_ground_collision;
    Sound wing_sfx;
    Sound hit_sfx;
    Sound die_sfx;

    Animation fly_anim;
    Sprite    sprite;

    Observers_List score_region_collision_observers;

    Phw_Character_Body *body;
} Bird;

Bird new_bird(Texture2D *, float x, float y, void *screen);

void update_bird(Bird *, bool allow_jump);

void draw_bird(Bird *);

void free_bird(Bird *);

void set_bird_position(Bird *, float x, float y);

Phw_Vec2 get_bird_position(const Bird *);

void reset_bird(Bird *);

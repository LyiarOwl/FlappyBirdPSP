#pragma once

#include "lframework/animation.h"
#include "lframework/sprite.h"
#include "pspgame/observers_list.h"

#include "phwind/phw_character_body.h"
#include "phwind/phw_vec2.h"

typedef void (*on_bird_score_region_collision_cb)(void);

typedef struct bird
{
    void          *screen;
    Phw_Vec2       initial_position;
    Phw_Vec2       vel;
    f32            grav;
    f32            max_vert_speed;
    f32            jump_impulse;
    f32            animation_elapsed;
    bool           entered_score_collision;
    Sprite         sprite;
    Animation      fly_anim;
    Observers_List score_region_collision_observers;

    Phw_Character_Body *body;
} Bird;

Bird new_bird(Texture2D *, Phw_Vec2 pos, void *screen);

void update_bird(Bird *);

void draw_bird(Bird *);

void free_bird(Bird *);

void set_bird_position(Bird *, Phw_Vec2);

Phw_Vec2 get_bird_position(const Bird *);

void reset_bird(Bird *);

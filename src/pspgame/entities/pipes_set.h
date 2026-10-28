#pragma once

#include "lframework/texture2d.h"
#include "phwind/phw_character_body.h"
#include "pipe.h"

typedef struct pipes_set
{
    Pipe lower_pipe;
    Pipe upper_pipe;
    f32  space_between;
    Vec2 pos;

    Phw_Character_Body *score_body;
} Pipes_Set;

Pipes_Set new_pipes_set(const Texture2D *, void *);

void draw_pipes_set(Pipes_Set *);

void set_pipes_set_position(Pipes_Set *, Vec2);

Vec2 get_pipes_set_position(const Pipes_Set *);

void set_enabled_pipes_set(Pipes_Set *, bool);

#pragma once

#include "lframework/sprite.h"
#include "phwind/phw_character_body.h"
typedef struct pipe
{
    void  *screen;
    Sprite sprite;

    Phw_Character_Body *body;
} Pipe;

void draw_pipe(Pipe *);

void set_pipe_position(Pipe *, Vec2);

Vec2 get_pipe_position(const Pipe *);

Pipe create_upper_pipe(const Texture2D *, void *screen);

Pipe create_lower_pipe(const Texture2D *, void *screen);

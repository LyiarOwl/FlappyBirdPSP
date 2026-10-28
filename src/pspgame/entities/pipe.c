#include "pipe.h"
#include "layers_mask.h"
#include "lframework/sprite.h"
#include "lframework/texture2d.h"
#include "lframework/texture_region.h"
#include "phwind/phw_character_body.h"
#include "phwind/phw_shapes.h"
#include "phwind/phw_world.h"
#include "pspgame/globals.h"

static Pipe new_pipe(void *screen)
{
    Pipe pipe   = { 0 };
    pipe.screen = screen;

    pipe.body = phw_world_create_character_body(
        &world, phw_create_rect_coll_shape((Phw_Vec2){ 26 / 2.f, 160 / 2.f })
    );
    pipe.body->user_layer_mask = LAYER_MASK_PIPE;

    return pipe;
}

void draw_pipe(Pipe *pipe)
{
    Vec2 pos       = get_pipe_position(pipe);
    pipe->sprite.x = pos.x;
    pipe->sprite.y = pos.y;
    sprite_draw(&pipe->sprite, sprite_batch);
}

void set_pipe_position(Pipe *pipe, Vec2 pos)
{
    phw_body_set_position(pipe->body, (Phw_Vec2){ pos.x, pos.y });
}

Vec2 get_pipe_position(const Pipe *pipe)
{
    return (Vec2){ pipe->body->bbox.position.x, pipe->body->bbox.position.y };
}

Pipe create_lower_pipe(const Texture2D *texture, void *screen)
{
    Pipe pipe   = new_pipe(screen);
    pipe.sprite = sprite_new(texture_region_create(texture, 387, 0, 26, 160));
    spr_center_origin(&pipe.sprite);
    return pipe;
}

Pipe create_upper_pipe(const Texture2D *texture, void *screen)
{
    Pipe pipe   = new_pipe(screen);
    pipe.sprite = sprite_new(texture_region_create(texture, 355, 0, 26, 160));
    spr_center_origin(&pipe.sprite);
    return pipe;
}

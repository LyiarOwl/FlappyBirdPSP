#include "pipes_set.h"
#include "layers_mask.h"
#include "phwind/phw_character_body.h"
#include "phwind/phw_shapes.h"
#include "phwind/phw_world.h"
#include "pipe.h"
#include "pspgame/globals.h"

Pipes_Set new_pipes_set(const Texture2D *texture, void *screen)
{
    Pipes_Set set     = { 0 };
    set.space_between = 50.f;
    set.lower_pipe    = create_lower_pipe(texture, screen);
    set.upper_pipe    = create_upper_pipe(texture, screen);

    set.score_body = phw_world_create_character_body(
        &world, phw_create_rect_coll_shape((Phw_Vec2){ 2.f, 25.f })
    );
    set.score_body->user_layer_mask = LAYER_MASK_SCORE;

    return set;
}

void draw_pipes_set(Pipes_Set *set)
{
    draw_pipe(&set->lower_pipe);
    draw_pipe(&set->upper_pipe);
}

void set_pipes_set_position(Pipes_Set *set, Vec2 pos)
{
    set->pos = pos;

    f32 pipe_h_height = set->lower_pipe.sprite.region.clip_rect.h * 0.5f;
    f32 half_gap      = set->space_between * 0.5f;

    // clang-format off
    set_pipe_position(&set->lower_pipe, (Vec2){
        pos.x,
        pos.y + pipe_h_height + half_gap
    });
    set_pipe_position(&set->upper_pipe, (Vec2){
        pos.x,
        pos.y - pipe_h_height - half_gap
    });

    phw_body_set_position(set->score_body, (Phw_Vec2){pos.x, pos.y});
    // clang-format on
}

Vec2 get_pipes_set_position(const Pipes_Set *set) { return set->pos; }

void set_enabled_pipes_set(Pipes_Set *set, bool state)
{
    set->lower_pipe.body->enabled = state;
    set->upper_pipe.body->enabled = state;
    set->score_body->enabled      = state;
}

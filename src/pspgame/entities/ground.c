#include "ground.h"
#include "lframework/colors.h"
#include "lframework/sprite_batch.h"
#include "lframework/texture_region.h"
#include "pspgame/globals.h"

Ground new_ground(Texture2D *texture)
{
    Ground ground = { 0 };
    ground.region = texture_region_create(texture, 292, 168, 168, 56);
    return ground;
}

void update_ground(Ground *ground) { ground->position.x -= ground->scroll_speed; }

void draw_ground(Ground *ground)
{
    sprite_batch_draw_tex_pro(
        g_sprite_batch,
        ground->region.texture,
        ground->position.x,
        ground->position.y,
        COLOR_WHITE,
        ground->region.clip_rect.x,
        ground->region.clip_rect.y,
        ground->region.clip_rect.w,
        ground->region.clip_rect.h,
        0.f,
        0.f,
        0.f,
        1.f,
        1.f,
        false,
        false
    );
}

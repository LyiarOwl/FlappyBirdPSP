#include "lframework/colors.h"
#include "lframework/sprite_batch.h"

#include "background.h"
#include "pspgame/globals.h"

Background create_background(Texture2D *texture)
{
    Background bg = { 0 };
    bg.region     = texture_region_create(texture, 0, 0, 144, 256);
    return bg;
}

void update_background(Background *bg) { bg->position.x -= bg->scroll_speed; }

void draw_background(Background *bg)
{
    // sprite_batch_draw_tex_region(sprite_batch, bg->region, bg->position, COLOR_WHITE);
    sprite_batch_draw_tex_pro(
        sprite_batch,
        bg->region.texture,
        bg->position.x,
        bg->position.y,
        COLOR_WHITE,
        bg->region.clip_rect.x,
        bg->region.clip_rect.y,
        bg->region.clip_rect.w,
        bg->region.clip_rect.h,
        0.f,
        0.f,
        0.f,
        1.f,
        1.f,
        false,
        false
    );
}

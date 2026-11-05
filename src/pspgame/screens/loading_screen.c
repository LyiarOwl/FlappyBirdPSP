#include "loading_screen.h"
#include "lframework/animation.h"
#include "lframework/colors.h"
#include "lframework/common.h"
#include "lframework/sprite.h"
#include "lframework/sprite_batch.h"
#include "lframework/texture2d.h"
#include "lframework/texture_region.h"
#include "pspgame/globals.h"
#include "pspgame/screen_manager.h"
#include "settings.h"

static color bg_color = 0;

static void enter(void *ctx, Screen_Manager *m)
{
    (void)m;

    Loading_Screen *s = ctx;

    bg_color = color_from_rgba(20, 52, 100, 255);

    s->delta       = 1.f / 60.f;
    s->spritesheet = texture2d_load("assets/images/spritesheet.png");

    const Vec2  center        = { SCREEN_WIDTH / 2.f, SCREEN_HEIGHT / 2.f };
    const float space_between = 10.f;
    const float dur           = 0.15f;
    const float scl           = 1.35f;

    // bird
    // clang-format off
    Texture_Region bird_frames[4] = {
        texture_region_create(&s->spritesheet, 423, 9, 19, 14),
        texture_region_create(&s->spritesheet, 455, 9, 19, 14),
        texture_region_create(&s->spritesheet, 487, 9, 19, 14),
        texture_region_create(&s->spritesheet, 455, 9, 19, 14)
    };
    // clang-format on
    s->bird_anim = animation_new(dur, bird_frames, 4);
    animation_set_play_mode(&s->bird_anim, ANIM_PLAY_MODE_LOOP);
    s->bird_sprite = sprite_new(bird_frames[0]);
    spr_center_origin(&s->bird_sprite);
    s->bird_sprite.x       = center.x;
    s->bird_sprite.y       = center.y - space_between;
    s->bird_sprite.scale_x = scl;
    s->bird_sprite.scale_y = scl;

    // loading text
    // clang-format off
    Texture_Region loading_frames[4] = {
        texture_region_create(&s->spritesheet, 244, 341, 50, 7),
        texture_region_create(&s->spritesheet, 244, 357, 50, 7),
        texture_region_create(&s->spritesheet, 244, 373, 50, 7),
        texture_region_create(&s->spritesheet, 244, 389, 50, 7)
    };
    // clang-format on
    s->loading_text_anim = animation_new(dur, loading_frames, 4);
    animation_set_play_mode(&s->loading_text_anim, ANIM_PLAY_MODE_LOOP);
    s->loading_text_sprite = sprite_new(loading_frames[0]);
    spr_center_origin(&s->loading_text_sprite);
    s->loading_text_sprite.x       = center.x;
    s->loading_text_sprite.y       = center.y + space_between;
    s->loading_text_sprite.scale_x = scl;
    s->loading_text_sprite.scale_y = scl;
}

static void update(void *ctx, Screen_Manager *m)
{
    (void)m;
    Loading_Screen *s = ctx;

    s->bird_anim_elapsed += s->delta;
    s->loading_text_elapsed += s->delta;

    s->bird_sprite.region = *anim_get_kf(&s->bird_anim, s->bird_anim_elapsed);

    s->loading_text_sprite.region = *anim_get_kf(&s->loading_text_anim, s->loading_text_elapsed);
}

static void draw(void *ctx, const Screen_Manager *m)
{
    (void)m;
    Loading_Screen *s = ctx;

    clear(bg_color);
    sprite_batch_begin(g_sprite_batch, NULL);

    sprite_draw(&s->bird_sprite, g_sprite_batch);
    sprite_draw(&s->loading_text_sprite, g_sprite_batch);

    sprite_batch_end(g_sprite_batch);
}

static void exit(void *ctx, Screen_Manager *m)
{
    (void)m;
    Loading_Screen *g = ctx;
    animation_free(&g->bird_anim);
    animation_free(&g->loading_text_anim);
    texture2d_unload(&g->spritesheet);
}

Screen_Desc loading_screen_init(Loading_Screen *s)
{
    s->desc = (Screen_Desc){ s, enter, update, draw, exit, NULL };
    return s->desc;
}

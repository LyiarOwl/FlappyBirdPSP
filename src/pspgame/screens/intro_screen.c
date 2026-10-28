#include "intro_screen.h"
#include "lframework/colors.h"
#include "lframework/common.h"
#include "lframework/sprite_batch.h"
#include "lframework/texture2d.h"
#include "pspgame/globals.h"
#include "pspgame/screen_manager.h"
#include "screen_ids.h"
#include "settings.h"

static void enter(void *ctx, Screen_Manager *m)
{
    (void)m;
    Intro_Screen *s = ctx;

    s->alpha = 0;
    s->timer = 0;
    s->phase = 0;
    s->delta = 1.f / 60.f;
}

static void update(void *ctx, Screen_Manager *m)
{
    Intro_Screen *s = ctx;
    if (s->phase == 0) {
        s->alpha += s->delta;
        if (s->alpha >= 1) {
            s->alpha = 1;
            s->phase = 1;
        }
    } else if (s->phase == 1) {
        s->timer += s->delta;
        if (s->timer >= 1)
            s->phase = 2;
    } else {
        s->alpha -= s->delta;
        if (s->alpha <= 0) {
            s->alpha = 0;
            screen_manager_request(m, SCR_MENU);
            l_printf("------------ intro screen fadeout end trying to switch to menu screen");
        }
    }
}

static void draw(void *ctx, const Screen_Manager *m)
{
    (void)m;
    Intro_Screen *s = ctx;
    clear(COLOR_BLACK);
    sprite_batch_begin(sprite_batch, NULL);
    sprite_batch_draw_tex(
        sprite_batch,
        &s->logo,
        (float)SCREEN_WIDTH * 0.5f - s->logo.w * 0.5f,
        (float)SCREEN_HEIGHT * 0.5f - s->logo.h * 0.5f,
        color_set_alpha(COLOR_WHITE, s->alpha)
    );
    sprite_batch_end(sprite_batch);
}

static void exit(void *ctx, Screen_Manager *m)
{
    (void)m;
    Intro_Screen *s = ctx;
    texture2d_unload(&s->logo);
    s->logo = (Texture2D){ 0 };
    l_printf("---------------------- unload intro");
}

void intro_screen_init(Intro_Screen *s)
{
    s->logo = texture2d_load("assets/images/logo.png");
    s->desc = (Screen_Desc){ s, enter, update, draw, exit, NULL };
}

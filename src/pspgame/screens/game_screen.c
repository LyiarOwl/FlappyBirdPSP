#include "game_screen.h"
#include "lframework/common.h"
#include "lframework/texture2d.h"
#include "screen_ids.h"

#define SPRITESHEET "assets/images/flappybird.png"

static Load_Result load(void *ctx, Screen_Manager *m)
{
    (void)m;
    Game_Screen *s = ctx;
    switch (s->load_step) {
    case 0:
        // s->main_tex = tex_man_load_tex(s->textures, MAIN_TEX);
        s->spritesheet = texture2d_load("assets/images/flappybird.png");
        if (!s->spritesheet.available)
            return LOAD_ERROR;
        s->load_step = 1;
        return LOAD_MORE;
    default:
        s->load_step = 0;
        return LOAD_DONE;
    }
}

static void update(void *ctx, Screen_Manager *m)
{
    (void)ctx;
    (void)m;
    // if (IsKeyPressed(KEY_F1))
    //     screen_manager_request(m, SCR_GAME1);
}

static void draw(void *ctx, const Screen_Manager *m)
{
    (void)m;
    Game_Screen *s = ctx;

    clear(COLOR_LIME);
}

static void exit_screen(void *ctx, Screen_Manager *m)
{
    (void)m;
    Game_Screen *s = ctx;
    texture2d_unload(&s->spritesheet);
    s->load_step = 0;
}

void game_screen_init(Game_Screen *s)
{
    s->desc = (Screen_Desc){ s, NULL, update, draw, exit_screen, load };
}

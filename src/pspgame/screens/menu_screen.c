#include "menu_screen.h"
#include "lframework/animation.h"
#include "lframework/bitmap_font.h"
#include "lframework/camera2d.h"
#include "lframework/colors.h"
#include "lframework/common.h"
#include "lframework/input.h"
#include "lframework/sprite.h"
#include "lframework/sprite_batch.h"
#include "lframework/texture2d.h"
#include "lframework/texture_region.h"
#include "pspctrl.h"
#include "pspgame/entities/backgrounds_controller.h"
#include "pspgame/entities/grounds_controller.h"
#include "pspgame/globals.h"
#include "pspgame/screen_manager.h"
#include "screen_ids.h"
#include "settings.h"

#define SPRITESHEET "assets/images/spritesheet.png"
#define GENERAL_FONT "assets/images/general_font.png"
#define DESCRIPTION "FAN GAME CREATED BY @LYIAR_U (v1.0.0)"

static bool analog_active      = false;
static f32  prev_current_btn_y = 0.f;

static Load_Result load(void *ctx, Screen_Manager *m)
{
    (void)m;
    Menu_Screen *s = ctx;

    switch (s->load_step++) {
    case 0:
        s->spritesheet = texture2d_load(SPRITESHEET);
        if (!s->spritesheet.available)
            return LOAD_ERROR;
        // s->load_step++;
        return LOAD_MORE;
    case 1:
        s->camera      = camera2d_new();
        s->camera.zoom = 1.1f;
        return LOAD_MORE;
    case 2:
        s->background_controller = new_backgrounds_controller(&s->spritesheet, 5, &s->camera, 0.5f);
        return LOAD_MORE;
    case 3: {
        Vec2 max = camera2d_screen_to_world(&s->camera, (Vec2){ SCREEN_WIDTH, SCREEN_HEIGHT });
        s->ground_controller = new_grounds_controller(
            &s->spritesheet, 4, &s->camera, 1.5f, (Vec2){ 0.f, max.y - 40.f }
        );
    }
        return LOAD_MORE;
    case 4:
        s->bg_color    = color_from_rgba(32, 214, 199, 255);
        s->black_color = color_from_rgba(20, 16, 19, 255);
        return LOAD_MORE;
    case 5: {
        Vec2 center =
            camera2d_screen_to_world(&s->camera, (Vec2){ SCREEN_WIDTH / 2.f, SCREEN_HEIGHT / 2.f });

        f32 scl = 1.25f;
        f32 gap = 30.f;

        s->game_logo_sprite = sprite_new(texture_region_create(&s->spritesheet, 19, 259, 91, 26));
        spr_center_origin(&s->game_logo_sprite);
        s->game_logo_sprite.x       = center.x;
        s->game_logo_sprite.y       = center.y - gap;
        s->game_logo_sprite.scale_x = scl;
        s->game_logo_sprite.scale_y = scl;

        s->play_btn_up   = texture_region_create(&s->spritesheet, 5, 287, 54, 33);
        s->play_btn_down = texture_region_create(&s->spritesheet, 5, 321, 54, 31);

        s->play_btn_sprite = sprite_new(s->play_btn_up);
        spr_center_origin(&s->play_btn_sprite);
        s->play_btn_sprite.x       = center.x - 40.f;
        s->play_btn_sprite.y       = center.y + gap;
        s->play_btn_sprite.scale_x = scl;
        s->play_btn_sprite.scale_y = scl;

        s->score_btn_up   = texture_region_create(&s->spritesheet, 69, 287, 54, 33);
        s->score_btn_down = texture_region_create(&s->spritesheet, 69, 321, 54, 31);

        s->score_btn_sprite = sprite_new(s->score_btn_up);
        spr_center_origin(&s->score_btn_sprite);
        s->score_btn_sprite.x       = center.x + 40.f;
        s->score_btn_sprite.y       = center.y + gap;
        s->score_btn_sprite.scale_x = scl;
        s->score_btn_sprite.scale_y = scl;

        s->options_count  = 2;
        s->current_option = 0;

        Texture_Region frames[2] = { texture_region_create(&s->spritesheet, 128, 256, 16, 16),
                                     texture_region_create(&s->spritesheet, 144, 256, 16, 16) };
        s->selector_anim         = animation_new(0.3f, frames, 2);
        animation_set_play_mode(&s->selector_anim, ANIM_PLAY_MODE_LOOP);
        s->selector_sprite = sprite_new(frames[0]);
        sprite_set_origin_norm(&s->selector_sprite, 0.55f, 0.5f);
        s->selector_sprite.x       = s->play_btn_sprite.x;
        s->selector_sprite.y       = s->play_btn_sprite.y - 28.f;
        s->selector_sprite.scale_x = scl;
        s->selector_sprite.scale_y = scl;
        s->selector_anim_elapsed   = 0.f;
        s->delta                   = 1.f / 60.f;
    }
        return LOAD_MORE;
    case 6: {
        s->general_font = bitmap_font_load(GENERAL_FONT, 8, 8, 0);

        s->screen_mid_top = camera2d_screen_to_world(&s->camera, (Vec2){ SCREEN_WIDTH / 2.f, 0 });
        s->description_str_size = bitmap_font_measure_str(&s->general_font, DESCRIPTION);

        s->screen_mid_bottom =
            camera2d_screen_to_world(&s->camera, (Vec2){ SCREEN_WIDTH / 2.f, SCREEN_HEIGHT });

        s->select_hint_sprite =
            sprite_new(texture_region_create(&s->spritesheet, 244, 275, 120, 11));
        s->confirm_hint_sprite =
            sprite_new(texture_region_create(&s->spritesheet, 244, 292, 89, 9));
        spr_center_origin(&s->select_hint_sprite);
        spr_center_origin(&s->confirm_hint_sprite);

        float scl = 1.25f;

        s->select_hint_sprite.scale_x = scl;
        s->select_hint_sprite.scale_y = scl;

        s->confirm_hint_sprite.scale_x = scl;
        s->confirm_hint_sprite.scale_y = scl;

        s->select_hint_sprite.x = s->screen_mid_bottom.x;
        s->select_hint_sprite.y = s->screen_mid_bottom.y - 35;

        s->confirm_hint_sprite.x = s->screen_mid_bottom.x;
        s->confirm_hint_sprite.y = s->screen_mid_bottom.y - 20;
    }
        return LOAD_MORE;
    default:
        s->load_step = 0;
        return LOAD_DONE;
    }
}

static void move_selector_left(Menu_Screen *g)
{
    g->current_option -= 1;
    if (g->current_option < 0)
        g->current_option = g->options_count - 1;
}
static void move_selector_right(Menu_Screen *g)
{
    g->current_option += 1;
    if (g->current_option >= g->options_count)
        g->current_option = 0;
}

static void update(void *ctx, Screen_Manager *m)
{
    (void)m;

    Menu_Screen *g = ctx;

    update_backgrounds_controller(&g->background_controller);
    update_grounds_controller(&g->ground_controller);

    // move cursor
    if (input_was_btn_pressed(&input, PSP_CTRL_RIGHT))
        move_selector_right(g);
    else if (input_was_btn_pressed(&input, PSP_CTRL_LEFT))
        move_selector_left(g);

    if (f32_abs(input.analog.x) > 0.25f) {
        if (!analog_active) {
            analog_active = true;
            if (input.analog.x > 0)
                move_selector_right(g);
            else if (input.analog.x < 0)
                move_selector_left(g);
        }
    } else {
        analog_active = false;
    }

    // update cursor position and assign proper up/down btn's sprite
    Sprite         *current_btn      = NULL;
    Texture_Region *current_btn_down = NULL;
    Texture_Region *current_btn_up   = NULL;
    switch (g->current_option) {
    case 0:
        g->selector_sprite.x = g->play_btn_sprite.x;

        current_btn      = &g->play_btn_sprite;
        current_btn_down = &g->play_btn_down;
        current_btn_up   = &g->play_btn_up;
        break;
    case 1:
        g->selector_sprite.x = g->score_btn_sprite.x;

        current_btn      = &g->score_btn_sprite;
        current_btn_down = &g->score_btn_down;
        current_btn_up   = &g->score_btn_up;
        break;
    }

    if (input_was_btn_pressed(&input, PSP_CTRL_CROSS)) {
        g->selector_anim_elapsed = 0.3f;
        current_btn->region      = *current_btn_down;
        current_btn->y += 2.f;
    }
    if (input_was_btn_released(&input, PSP_CTRL_CROSS)) {
        current_btn->region = *current_btn_up;
        current_btn->y -= 2.f;
    }

    if (input_is_btn_up(&input, PSP_CTRL_CROSS))
        g->selector_anim_elapsed += g->delta;

    g->selector_sprite.region =
        *animation_get_keyframe(&g->selector_anim, g->selector_anim_elapsed);

    // if (IsKeyPressed(KEY_F2))
    //     screen_manager_request(m, SCR_GAME);
}

static void draw(void *ctx, const Screen_Manager *m)
{
    (void)m;
    Menu_Screen *s = ctx;
    clear(s->bg_color);

    sprite_batch_begin(sprite_batch, &s->camera);

    draw_backgrounds_controller(&s->background_controller);
    draw_grounds_controller(&s->ground_controller);

    sprite_draw(&s->game_logo_sprite, sprite_batch);
    sprite_draw(&s->play_btn_sprite, sprite_batch);
    sprite_draw(&s->score_btn_sprite, sprite_batch);
    sprite_draw(&s->selector_sprite, sprite_batch);
    sprite_draw(&s->select_hint_sprite, sprite_batch);
    sprite_draw(&s->confirm_hint_sprite, sprite_batch);

    sprite_batch_end(sprite_batch);
}
static void exit_screen(void *ctx, Screen_Manager *m)
{
    (void)m;
    Menu_Screen *s = ctx;
    free_grounds_controller(&s->ground_controller);
    free_backgrounds_controller(&s->background_controller);
    bitmap_font_unload(&s->general_font);
    texture2d_unload(&s->spritesheet);
    s->load_step = 0;
}
Screen_Desc menu_screen_init(Menu_Screen *s)
{
    s->desc      = (Screen_Desc){ s, NULL, update, draw, exit_screen, load };
    s->load_step = 0;
    return s->desc;
}

#include "game_screen.h"
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
#include "phwind/phw_debug_renderer.h"
#include "pspctrl.h"
#include "pspgame/entities/backgrounds_controller.h"
#include "pspgame/entities/bird.h"
#include "pspgame/entities/grounds_controller.h"
#include "pspgame/entities/pipes_controller.h"
#include "pspgame/globals.h"
#include "pspgame/observers_list.h"
#include "pspgame/screen_manager.h"
#include "screen_ids.h"
#include "settings.h"

#define SPRITESHEET "assets/images/flappybird.png"

static void add_score(void *user_data)
{
    Game_Screen *s = user_data;
    s->main_score += 1;
}

static Load_Result load(void *ctx, Screen_Manager *m)
{
    (void)m;
    Game_Screen *s = ctx;
    switch (s->load_step++) {
    case 0:
        s->spritesheet = texture2d_load("assets/images/spritesheet.png");
        if (!s->spritesheet.available)
            return LOAD_ERROR;
        return LOAD_MORE;
    case 1:
        s->camera          = camera2d_new();
        s->camera.offset_x = 0.f;
        s->camera.offset_y = 0.f;
        s->camera.zoom     = 1.1f;

        s->background_contrl = new_backgrounds_controller(&s->spritesheet, 4, &s->camera, 0.25f);
        return LOAD_MORE;
    case 2:
        s->foreground_scroll_speed = 1.5f;
        s->clear_color             = color_from_rgba(132, 155, 228, 255);

        s->ground_contrl = new_grounds_controller(
            &s->spritesheet,
            4,
            &s->camera,
            s->foreground_scroll_speed,
            (Vec2){ 0, SCREEN_HEIGHT - 70 }
        );
        return LOAD_MORE;
    case 3: {
        Vec2 screen_max =
            camera2d_screen_to_world(&s->camera, (Vec2){ SCREEN_WIDTH, SCREEN_HEIGHT });
        Vec2 screen_center =
            camera2d_screen_to_world(&s->camera, (Vec2){ SCREEN_WIDTH / 2.f, SCREEN_HEIGHT / 2.f });

        s->pipes_contrl = new_pipes_controller(
            &s->spritesheet, 1.2f, 7, (Vec2){ screen_max.x, screen_center.y }, &s->camera, s
        );
        s->pipes_contrl.scroll_speed = s->foreground_scroll_speed;
    }
        return LOAD_MORE;
    case 4: {
        s->debug_renderer = phw_debug_renderer_new(&world, 1024UL);
        return LOAD_MORE;
    }
    case 5: {
        s->screen_center =
            camera2d_screen_to_world(&s->camera, (Vec2){ SCREEN_WIDTH / 2.f, SCREEN_HEIGHT / 2.f });
        s->bird = new_bird(&s->spritesheet, s->screen_center.x, s->screen_center.y, s);
    }
        return LOAD_MORE;
    case 6: {
        s->delta = 1.f / 60.f;
        s->scale = 1.25f;

        Vec2 screen_center =
            camera2d_screen_to_world(&s->camera, (Vec2){ SCREEN_WIDTH / 2.f, SCREEN_HEIGHT / 2.f });

        float gap       = 50.f;
        float hints_gap = 15.f;

        // upper hint
        float lower_hint_y_offset = 10.f;
        s->jump_hint_upper_sprite =
            sprite_new(texture_region_create(&s->spritesheet, 124, 370, 21, 28));
        spr_center_origin(&s->jump_hint_upper_sprite);
        s->jump_hint_upper_sprite.scale_x = s->scale;
        s->jump_hint_upper_sprite.scale_y = s->scale;
        s->jump_hint_upper_sprite.x       = screen_center.x;
        s->jump_hint_upper_sprite.y       = screen_center.y + gap - hints_gap + lower_hint_y_offset;

        // lower hint
        Texture_Region lower_hint_frames[2] = {
            texture_region_create(&s->spritesheet, 145, 367, 77, 15),
            texture_region_create(&s->spritesheet, 145, 383, 77, 15)
        };

        s->jump_hint_lower_anim = animation_new(0.2f, lower_hint_frames, 2);
        animation_set_play_mode(&s->jump_hint_lower_anim, ANIM_PLAY_MODE_LOOP);
        s->jump_hint_lower_sprite = sprite_new(lower_hint_frames[0]);
        spr_center_origin(&s->jump_hint_lower_sprite);
        s->jump_hint_lower_sprite.scale_x = s->scale;
        s->jump_hint_lower_sprite.scale_y = s->scale;
        s->jump_hint_lower_sprite.x       = screen_center.x;
        s->jump_hint_lower_sprite.y       = screen_center.y + gap + hints_gap + lower_hint_y_offset;

        // get ready
        s->get_ready_label_sprite =
            sprite_new(texture_region_create(&s->spritesheet, 137, 292, 94, 27));
        spr_center_origin(&s->get_ready_label_sprite);
        s->get_ready_label_sprite.scale_x = s->scale;
        s->get_ready_label_sprite.scale_y = s->scale;
        s->get_ready_label_sprite.x       = screen_center.x;
        s->get_ready_label_sprite.y       = screen_center.y - gap;
    }
        return LOAD_MORE;
    case 7: {
        s->main_score_font = bitmap_font_load("assets/images/score_font.png", 24, 24, -8);

        s->main_score = 0;

        s->screen_top_center =
            camera2d_screen_to_world(&s->camera, (Vec2){ SCREEN_WIDTH / 2.f, 0 });

        observers_list_add_observer(&s->bird.score_region_collision_observers, add_score);
    }
        return LOAD_MORE;
    case 8: {
        s->game_over_label_sprite =
            sprite_new(texture_region_create(&s->spritesheet, 135, 325, 99, 22));
        s->score_board_sprite = sprite_new(texture_region_create(&s->spritesheet, 7, 353, 115, 61));
        spr_center_origin(&s->game_over_label_sprite);
        spr_center_origin(&s->score_board_sprite);

        s->game_over_label_sprite.scale_x = s->scale;
        s->game_over_label_sprite.scale_y = s->scale;

        s->score_board_sprite.scale_x = s->scale;
        s->score_board_sprite.scale_y = s->scale;

        s->game_over_label_sprite.x = s->score_board_sprite.x = s->screen_center.x;
        s->game_over_label_sprite.y = s->score_board_sprite.y = s->screen_center.y;

        s->game_over_label_sprite.y -= 60.f;
    }
        return LOAD_MORE;
    default:
        s->load_step = 0;
        return LOAD_DONE;
    }
}

static void update(void *ctx, Screen_Manager *m)
{
    (void)m;
    Game_Screen *g = ctx;

    // start and show hint
    if (!g->start) {
        if (input_was_btn_pressed(&input, PSP_CTRL_CROSS)) {
            g->start = true;
        }

        g->jump_hint_lower_elapsed += g->delta;
        g->jump_hint_lower_sprite.region =
            *anim_get_kf(&g->jump_hint_lower_anim, g->jump_hint_lower_elapsed);
    }

    if (!g->game_over) {
        update_backgrounds_controller(&g->background_contrl);
        update_grounds_controller(&g->ground_contrl);
    }

    if (g->start) {
        if (!g->game_over) {
            update_pipes_controller(&g->pipes_contrl);
        }

        update_bird(&g->bird);
    }

    // if (IsKeyPressed(KEY_F1))
    //     screen_manager_request(m, SCR_GAME1);
}

static void draw(void *ctx, const Screen_Manager *m)
{
    (void)m;
    Game_Screen *s = ctx;

    clear(s->clear_color);
    sprite_batch_begin(sprite_batch, &s->camera);

    draw_backgrounds_controller(&s->background_contrl);
    draw_pipes_controller(&s->pipes_contrl);
    draw_grounds_controller(&s->ground_contrl);

    draw_bird(&s->bird);

    if (!s->start) {
        sprite_draw(&s->jump_hint_upper_sprite, sprite_batch);
        sprite_draw(&s->jump_hint_lower_sprite, sprite_batch);
        sprite_draw(&s->get_ready_label_sprite, sprite_batch);
    }

    char text_buf[16];
    snprintf(text_buf, 16, "%d", s->main_score);
    Vec2 str_size = bitmap_font_measure_str(&s->main_score_font, text_buf);
    sprite_batch_draw_str(
        sprite_batch,
        &s->main_score_font,
        text_buf,
        s->screen_top_center.x - str_size.x * 0.5f,
        s->screen_top_center.y + 20.f,
        COLOR_WHITE
    );

    if (s->start) {
        if (s->game_over) {
            sprite_draw(&s->game_over_label_sprite, sprite_batch);
            sprite_draw(&s->score_board_sprite, sprite_batch);
        }
    }

    sprite_batch_end(sprite_batch);

    phw_debug_renderer_draw(s->debug_renderer, &s->camera);
}

static void exit_screen(void *ctx, Screen_Manager *m)
{
    (void)m;
    Game_Screen *s = ctx;
    free_backgrounds_controller(&s->background_contrl);
    free_grounds_controller(&s->ground_contrl);
    free_pipes_controller(&s->pipes_contrl);
    free_bird(&s->bird);
    animation_free(&s->jump_hint_lower_anim);
    bitmap_font_unload(&s->main_score_font);
    texture2d_unload(&s->spritesheet);
    phw_debug_renderer_free(s->debug_renderer);
    s->load_step = 0;
}

Screen_Desc game_screen_init(Game_Screen *s)
{
    s->desc = (Screen_Desc){ s, NULL, update, draw, exit_screen, load };
    return s->desc;
}

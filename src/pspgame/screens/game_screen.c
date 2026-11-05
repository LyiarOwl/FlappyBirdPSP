#include "game_screen.h"
#include "SDL3_mixer/SDL_mixer.h"
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
#include "phwind/phw_spatial_grid.h"
#include "phwind/phw_spatial_grid_bucket.h"
#include "phwind/phw_world.h"
#include "pspctrl.h"
#include "pspgame/entities/backgrounds_controller.h"
#include "pspgame/entities/bird.h"
#include "pspgame/entities/grounds_controller.h"
#include "pspgame/entities/pipes_controller.h"
#include "pspgame/globals.h"
#include "pspgame/observers_list.h"
#include "pspgame/screen_manager.h"
#include "pspintrman.h"
#include "screen_ids.h"
#include "settings.h"
#include <stdio.h>

#define SPRITESHEET "assets/images/flappybird.png"

typedef enum { IN_GAME, GAME_OVER, PAUSED } SCREEN_STATE;

static SCREEN_STATE curr_screen_state = IN_GAME;

static float btn_pressed_y_offset         = 0.f;
static float sub_score_elapsed            = 0.f;
static float sub_score_increment_interval = 0.05f;
static int   current_sub_score            = 0;
static bool  analog_active                = false;
static bool  running_sub_score_transition = false;

static void reset(Game_Screen *s)
{
    reset_pipes_controller(&s->pipes_contrl);
    reset_backgrounds_controller(&s->background_contrl);
    reset_grounds_controller(&s->ground_contrl);
    reset_bird(&s->bird);
    s->main_score                = 0;
    s->start                     = false;
    s->show_medal                = false;
    s->show_new_label            = false;
    curr_screen_state            = IN_GAME;
    sub_score_elapsed            = 0.f;
    current_sub_score            = 0;
    analog_active                = false;
    running_sub_score_transition = false;
}

static void move_selector_right(Game_Screen *s)
{
    s->current_option += 1;
    if (s->current_option >= s->options_count)
        s->current_option = 0;
}
static void move_selector_left(Game_Screen *s)
{
    s->current_option -= 1;
    if (s->current_option < 0)
        s->current_option = s->options_count - 1;
}

static void change_screen_state(SCREEN_STATE id, Game_Screen *s)
{
    if (curr_screen_state == id)
        return;

    curr_screen_state = id;
    switch (curr_screen_state) {
    case IN_GAME:
        break;
    case GAME_OVER:
        break;
    case PAUSED:
        break;
    }
}

void game_screen_switch_to_game_over(void *s) { change_screen_state(GAME_OVER, s); }

static void add_score(void *user_data)
{
    Game_Screen *s = user_data;
    s->main_score += 1;
    MIX_StopTrack(s->point_sfx.track, 0);
    MIX_PlayTrack(s->point_sfx.track, 0);
}

/* =================================================
 * ------------- menu buttons overlay --------------
 * =================================================
 */
static void update_buttons_overlay(Game_Screen *s, Screen_Manager *m)
{
    float btn_y_offset      = 0.f;
    float selector_y_offset = 0.f;

    s->confirm_action_hint_sprite.x = s->screen_bottom_center.x;
    s->confirm_action_hint_sprite.y = s->screen_bottom_center.y - 10.f;
    s->move_selector_hint_sprite.x  = s->screen_bottom_center.x;
    s->move_selector_hint_sprite.y  = s->screen_bottom_center.y - 20.f;
    s->resume_hint_sprite.x         = s->screen_bottom_center.x;
    s->resume_hint_sprite.y         = s->screen_bottom_center.y - 30.f;

    switch (curr_screen_state) {
    case IN_GAME:
        break;
    case GAME_OVER:
        btn_y_offset      = 70.f;
        selector_y_offset = 27.f;

        break;
    case PAUSED:
        selector_y_offset = 27.f;
        break;
    }

    // move selector
    if (input_is_btn_up(&g_input, PSP_CTRL_CROSS)) {
        if (input_was_btn_pressed(&g_input, PSP_CTRL_RIGHT))
            move_selector_right(s);
        else if (input_was_btn_pressed(&g_input, PSP_CTRL_LEFT))
            move_selector_left(s);

        if (f32_abs(g_input.analog.x) > 0.25f) {
            if (!analog_active) {
                analog_active = true;
                if (g_input.analog.x > 0)
                    move_selector_right(s);
                else if (g_input.analog.x < 0)
                    move_selector_left(s);
            }
        } else {
            analog_active = false;
        }
    }

    // setting current btn (visuals)
    Texture_Region *current_btn_down = NULL;
    Texture_Region *current_btn_up   = NULL;
    Sprite         *current_btn      = NULL;
    switch (s->current_option) {
    case 0:
        current_btn_down     = &s->menu_btn_down;
        current_btn_up       = &s->menu_btn_up;
        current_btn          = &s->menu_btn_sprite;
        s->selector_sprite.x = s->menu_btn_sprite.x;
        s->selector_sprite.y = s->menu_btn_sprite.y - selector_y_offset;
        break;
    case 1:
        current_btn_down     = &s->restart_btn_down;
        current_btn_up       = &s->restart_btn_up;
        current_btn          = &s->restart_btn_sprite;
        s->selector_sprite.x = s->restart_btn_sprite.x;
        s->selector_sprite.y = s->restart_btn_sprite.y - selector_y_offset;
        break;
    }

    // pressing (visuals)
    if (input_was_btn_pressed(&g_input, PSP_CTRL_CROSS)) {
        s->selector_anim_elapsed = 0.3f;
        current_btn->region      = *current_btn_down;
        btn_pressed_y_offset     = 2.f;
    }
    if (input_was_btn_released(&g_input, PSP_CTRL_CROSS)) {
        current_btn->region      = *current_btn_up;
        btn_pressed_y_offset     = 0;
        s->selector_anim_elapsed = 0.f;
    }

    // actions
    if (curr_screen_state == PAUSED || curr_screen_state == GAME_OVER) {
        if (input_was_btn_pressed(&g_input, PSP_CTRL_CROSS)) {
            MIX_StopTrack(s->wing_sfx.track, 0);
            MIX_PlayTrack(s->wing_sfx.track, 0);
        }
        if (input_was_btn_released(&g_input, PSP_CTRL_CROSS)) {

            switch (s->current_option) {
            case 0:
                screen_manager_request(m, SCR_MENU);
                break;
            case 1:
                reset(s);
                break;
            }
        }
    }

    // animating selector
    if (input_is_btn_up(&g_input, PSP_CTRL_CROSS))
        s->selector_anim_elapsed += s->delta;

    s->selector_sprite.region =
        *animation_get_keyframe(&s->selector_anim, s->selector_anim_elapsed);

    // position buttons
    const float space_between = 40.f;
    s->menu_btn_sprite.x      = s->screen_center.x - space_between;
    s->menu_btn_sprite.y      = s->screen_center.y + btn_y_offset;

    s->restart_btn_sprite.x = s->screen_center.x + space_between;
    s->restart_btn_sprite.y = s->screen_center.y + btn_y_offset;

    current_btn->y += btn_pressed_y_offset;
}

static void draw_buttons_overlay(Game_Screen *s)
{
    sprite_draw(&s->restart_btn_sprite, g_sprite_batch);
    sprite_draw(&s->menu_btn_sprite, g_sprite_batch);

    sprite_draw(&s->selector_sprite, g_sprite_batch);

    sprite_draw(&s->move_selector_hint_sprite, g_sprite_batch);
    sprite_draw(&s->confirm_action_hint_sprite, g_sprite_batch);
    if (curr_screen_state == PAUSED)
        sprite_draw(&s->resume_hint_sprite, g_sprite_batch);
}

/* =================================================
 * --------------- screen life cycle ---------------
 * =================================================
 */
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
        s->debug_renderer = phw_debug_renderer_new(&g_world, 1024UL);
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

        s->game_over_label_sprite.y -= 65.f;
    }
        return LOAD_MORE;
    case 9: {
        s->restart_btn_down = texture_region_create(&s->spritesheet, 133, 433, 54, 31);
        s->restart_btn_up   = texture_region_create(&s->spritesheet, 133, 399, 54, 33);

        s->menu_btn_down = texture_region_create(&s->spritesheet, 197, 433, 54, 31);
        s->menu_btn_up   = texture_region_create(&s->spritesheet, 197, 399, 54, 33);

        s->restart_btn_sprite = sprite_new(s->restart_btn_up);
        s->menu_btn_sprite    = sprite_new(s->menu_btn_up);
        spr_center_origin(&s->restart_btn_sprite);
        spr_center_origin(&s->menu_btn_sprite);
        s->restart_btn_sprite.scale_x = s->scale;
        s->restart_btn_sprite.scale_y = s->scale;
        s->menu_btn_sprite.scale_x    = s->scale;
        s->menu_btn_sprite.scale_y    = s->scale;

        // selector
        s->options_count  = 2;
        s->current_option = 1;

        Texture_Region frames[2] = { texture_region_create(&s->spritesheet, 128, 256, 16, 16),
                                     texture_region_create(&s->spritesheet, 144, 256, 16, 16) };
        s->selector_anim         = animation_new(0.3f, frames, 2);
        animation_set_play_mode(&s->selector_anim, ANIM_PLAY_MODE_LOOP);
        s->selector_sprite = sprite_new(frames[0]);
        sprite_set_origin_norm(&s->selector_sprite, 0.55f, 0.5f);
        s->selector_sprite.scale_x = s->scale;
        s->selector_sprite.scale_y = s->scale;
        s->selector_anim_elapsed   = 0.f;
    }
        return LOAD_MORE;
    case 10: {
        s->move_selector_hint_sprite =
            sprite_new(texture_region_create(&s->spritesheet, 244, 275, 120, 11));
        s->confirm_action_hint_sprite =
            sprite_new(texture_region_create(&s->spritesheet, 244, 292, 89, 9));
        s->pause_hint_sprite = sprite_new(texture_region_create(&s->spritesheet, 372, 277, 81, 7));
        s->resume_hint_sprite =
            sprite_new(texture_region_create(&s->spritesheet, 372, 308, 115, 9));

        spr_center_origin(&s->move_selector_hint_sprite);
        spr_center_origin(&s->confirm_action_hint_sprite);
        spr_center_origin(&s->pause_hint_sprite);
        spr_center_origin(&s->resume_hint_sprite);

        s->screen_bottom_center =
            camera2d_screen_to_world(&s->camera, (Vec2){ SCREEN_WIDTH / 2.f, SCREEN_HEIGHT });

        s->pause_hint_sprite.x = s->screen_bottom_center.x;
        s->pause_hint_sprite.y = s->screen_bottom_center.y - 10.f;
    }
        return LOAD_MORE;
    case 11: {
        s->sub_score_font = bitmap_font_load("assets/images/score2_font.png", 7, 10, 1);
    }
        return LOAD_MORE;
    case 12: {
        s->new_label_sprite = sprite_new(texture_region_create(&s->spritesheet, 159, 260, 18, 9));

        s->bronze_medal_region   = texture_region_create(&s->spritesheet, 4, 420, 24, 24);
        s->silver_medal_region   = texture_region_create(&s->spritesheet, 36, 420, 24, 24);
        s->gold_medal_region     = texture_region_create(&s->spritesheet, 68, 420, 24, 24);
        s->platinum_medal_region = texture_region_create(&s->spritesheet, 100, 420, 24, 24);

        s->medal_sprite = sprite_new(s->bronze_medal_region);

        spr_center_origin(&s->new_label_sprite);
        spr_center_origin(&s->medal_sprite);

        s->medal_sprite.scale_x = s->medal_sprite.scale_y = s->scale;
        s->new_label_sprite.scale_x = s->new_label_sprite.scale_y = s->scale;

        s->medal_sprite.x = s->screen_center.x - 41.f;
        s->medal_sprite.y = s->screen_center.y + 3.f;

        s->new_label_sprite.x = s->screen_center.x + 17.f;
        s->new_label_sprite.y = s->screen_center.y - 22.f;
    }
        return LOAD_MORE;
    case 13:
        s->point_sfx = sound_new(g_mixer, "assets/sounds/sfx_point.mp3");
        s->wing_sfx  = sound_new(g_mixer, "assets/sounds/sfx_wing.mp3");
        return LOAD_MORE;
    default:
        s->load_step = 0;
        return LOAD_DONE;
    }
}

static void enter(void *ctx, Screen_Manager *m)
{
    (void)m;
    Game_Screen *g               = ctx;
    analog_active                = false;
    btn_pressed_y_offset         = 0.f;
    current_sub_score            = 0;
    curr_screen_state            = IN_GAME;
    running_sub_score_transition = false;
    g->show_medal                = false;
    g->show_new_label            = false;
    g->first_best_score          = false;
    // g->best_score        = 0;
}

static void update(void *ctx, Screen_Manager *m)
{
    (void)m;
    Game_Screen *g = ctx;

    switch (curr_screen_state) {
    case IN_GAME: {
        if (!g->start) {
            if (input_was_btn_pressed(&g_input, PSP_CTRL_CROSS)) {
                g->start = true;
            }

            g->jump_hint_lower_elapsed += g->delta;
            g->jump_hint_lower_sprite.region =
                *anim_get_kf(&g->jump_hint_lower_anim, g->jump_hint_lower_elapsed);
        }

        update_backgrounds_controller(&g->background_contrl);
        update_grounds_controller(&g->ground_contrl);

        if (g->start) {
            update_pipes_controller(&g->pipes_contrl);

            update_bird(&g->bird, true);

            if (input_was_btn_pressed(&g_input, PSP_CTRL_START)) {
                curr_screen_state = PAUSED;
            }
        }
    } break;
    case GAME_OVER: {
        if (!g->start)
            break;

        update_bird(&g->bird, false);

        if (!running_sub_score_transition)
            update_buttons_overlay(g, m);

        if (current_sub_score < g->main_score) {
            sub_score_elapsed += g->delta;
            if (sub_score_elapsed >= sub_score_increment_interval) {
                sub_score_elapsed -= sub_score_increment_interval;
                running_sub_score_transition = true;
                current_sub_score++;

                if (current_sub_score > g->main_score) {
                    current_sub_score = g->main_score;
                }
            }
        } else {
            if (g->main_score > g->best_score) {
                g->best_score = g->main_score;

                if (!g->first_best_score) {
                    g->first_best_score = true;
                } else {
                    g->show_new_label = true;
                }
            }
            running_sub_score_transition = false;
        }

        if (g->best_score >= 10) {
            g->medal_sprite.region = g->bronze_medal_region;
            if (g->best_score >= 20)
                g->medal_sprite.region = g->silver_medal_region;
            else if (g->best_score >= 30)
                g->medal_sprite.region = g->gold_medal_region;
            else if (g->best_score >= 40)
                g->medal_sprite.region = g->platinum_medal_region;

            g->show_medal = true;
        }
    } break;
    case PAUSED: {
        if (!g->start)
            break;

        if (input_was_btn_pressed(&g_input, PSP_CTRL_START) ||
            input_was_btn_pressed(&g_input, PSP_CTRL_CIRCLE)) {
            curr_screen_state = IN_GAME;
        }
        update_buttons_overlay(g, m);
    } break;
    }

    // if (IsKeyPressed(KEY_F1))
    //     screen_manager_request(m, SCR_GAME1);
}

static void draw(void *ctx, const Screen_Manager *m)
{
    (void)m;
    Game_Screen *s = ctx;

    clear(s->clear_color);
    sprite_batch_begin(g_sprite_batch, &s->camera);

    draw_backgrounds_controller(&s->background_contrl);
    draw_pipes_controller(&s->pipes_contrl);
    draw_grounds_controller(&s->ground_contrl);

    draw_bird(&s->bird);

    if (!s->start) {
        sprite_draw(&s->jump_hint_upper_sprite, g_sprite_batch);
        sprite_draw(&s->jump_hint_lower_sprite, g_sprite_batch);
        sprite_draw(&s->get_ready_label_sprite, g_sprite_batch);
    }

    switch (curr_screen_state) {
    case IN_GAME: {
        if (s->start) {
            char text_buf[16];
            snprintf(text_buf, 16, "%d", s->main_score);
            Vec2 str_size = bitmap_font_measure_str(&s->main_score_font, text_buf, 1.f, 1.f);
            sprite_batch_draw_str(
                g_sprite_batch,
                &s->main_score_font,
                text_buf,
                s->screen_top_center.x - str_size.x * 0.5f,
                s->screen_top_center.y + 20.f,
                1.f,
                1.f,
                COLOR_WHITE
            );
        }

        sprite_draw(&s->pause_hint_sprite, g_sprite_batch);
    }

    break;
    case GAME_OVER: {
        sprite_draw(&s->game_over_label_sprite, g_sprite_batch);
        sprite_draw(&s->score_board_sprite, g_sprite_batch);

        float x_offset = 57.f;
        char  score_buf[16];
        snprintf(score_buf, 16, "%d", current_sub_score);
        Vec2 score_text_size = bitmap_font_measure_str(&s->sub_score_font, score_buf, 1.f, 1.f);
        sprite_batch_draw_str(
            g_sprite_batch,
            &s->sub_score_font,
            score_buf,
            s->screen_center.x + x_offset - score_text_size.x,
            s->screen_center.y - 16.f,
            1.f,
            1.f,
            COLOR_WHITE
        );

        snprintf(score_buf, 16, "%d", s->best_score);
        score_text_size = bitmap_font_measure_str(&s->sub_score_font, score_buf, 1.5f, 1.5f);
        sprite_batch_draw_str(
            g_sprite_batch,
            &s->sub_score_font,
            score_buf,
            s->screen_center.x + x_offset - score_text_size.x,
            s->screen_center.y + 10.f,
            1.5f,
            1.5f,
            COLOR_WHITE
        );

        if (s->show_new_label)
            sprite_draw(&s->new_label_sprite, g_sprite_batch);

        if (s->show_medal)
            sprite_draw(&s->medal_sprite, g_sprite_batch);

        draw_buttons_overlay(s);
    } break;
    case PAUSED:
        draw_buttons_overlay(s);
        // sprite_draw(&s->spri, g_Sprite_Batch *)
        break;
    }

    sprite_batch_end(g_sprite_batch);

    // phw_debug_renderer_draw(s->debug_renderer, &s->camera);
}

static void exit_screen(void *ctx, Screen_Manager *m)
{
    (void)m;
    Game_Screen *s = ctx;
    free_backgrounds_controller(&s->background_contrl);
    free_grounds_controller(&s->ground_contrl);
    free_pipes_controller(&s->pipes_contrl);
    free_bird(&s->bird);
    sound_free(&s->point_sfx);
    sound_free(&s->wing_sfx);
    animation_free(&s->jump_hint_lower_anim);
    bitmap_font_unload(&s->main_score_font);
    bitmap_font_unload(&s->sub_score_font);
    texture2d_unload(&s->spritesheet);
    phw_debug_renderer_free(s->debug_renderer);
    phw_world_clear(&g_world);
    s->load_step      = 0;
    s->start          = false;
    curr_screen_state = IN_GAME;
}

Screen_Desc game_screen_init(Game_Screen *s)
{
    s->desc = (Screen_Desc){ s, enter, update, draw, exit_screen, load };
    return s->desc;
}

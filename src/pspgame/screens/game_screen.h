#ifndef GAME2_SCREEN_H
#define GAME2_SCREEN_H

#include "SDL3_mixer/SDL_mixer.h"
#include "lframework/animation.h"
#include "lframework/camera2d.h"
#include "lframework/sprite.h"
#include "lframework/texture2d.h"
#include "lframework/texture_region.h"
#include "phwind/phw_debug_renderer.h"
#include "pspgame/entities/backgrounds_controller.h"
#include "pspgame/entities/bird.h"
#include "pspgame/entities/grounds_controller.h"
#include "pspgame/entities/pipes_controller.h"
#include "pspgame/screen_manager.h"
#include "pspgame/sound.h"

typedef struct
{
    Screen_Desc desc;
    Texture2D   spritesheet;
    Bitmap_Font main_score_font;
    Bitmap_Font sub_score_font;

    Camera2D               camera;
    Backgrounds_Controller background_contrl;
    Grounds_Controller     ground_contrl;
    Pipes_Controller       pipes_contrl;
    Bird                   bird;

    Sprite jump_hint_upper_sprite;
    Sprite jump_hint_lower_sprite;

    Sprite get_ready_label_sprite;
    Sprite game_over_label_sprite;
    Sprite new_label_sprite;
    Sprite medal_sprite;

    Sprite score_board_sprite;

    Sprite restart_btn_sprite;
    Sprite menu_btn_sprite;

    Sprite move_selector_hint_sprite;
    Sprite confirm_action_hint_sprite;
    Sprite pause_hint_sprite;
    Sprite resume_hint_sprite;

    Sprite selector_sprite;

    Texture_Region bronze_medal_region;
    Texture_Region silver_medal_region;
    Texture_Region gold_medal_region;
    Texture_Region platinum_medal_region;
    Texture_Region restart_btn_down;
    Texture_Region restart_btn_up;
    Texture_Region menu_btn_down;
    Texture_Region menu_btn_up;

    Animation jump_hint_lower_anim;
    Animation selector_anim;

    Vec2 screen_top_center;
    Vec2 screen_center;
    Vec2 screen_bottom_center;

    Phw_Debug_Renderer *debug_renderer;

    Sound point_sfx;
    Sound wing_sfx;

    bool start;
    bool show_new_label;
    bool show_medal;
    bool first_best_score;

    color clear_color;

    int load_step;
    int main_score;
    int best_score;
    int current_option;
    int options_count;

    float selector_anim_elapsed;
    float jump_hint_lower_elapsed;

    float foreground_scroll_speed;
    float scale;
    float delta;
} Game_Screen;

Screen_Desc game_screen_init(Game_Screen *s);

void game_screen_switch_to_game_over(void *screen);

#endif

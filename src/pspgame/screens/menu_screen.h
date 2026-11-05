#ifndef GAME1_SCREEN_H
#define GAME1_SCREEN_H

#include "lframework/animation.h"
#include "lframework/bitmap_font.h"
#include "lframework/camera2d.h"
#include "lframework/colors.h"
#include "lframework/sprite.h"
#include "lframework/texture2d.h"
#include "lframework/texture_region.h"
#include "pspgame/entities/backgrounds_controller.h"
#include "pspgame/entities/grounds_controller.h"
#include "pspgame/screen_manager.h"
#include "pspgame/sound.h"

typedef struct
{
    Screen_Desc desc;

    Texture2D   spritesheet;
    Bitmap_Font general_font;

    Camera2D camera;

    Backgrounds_Controller background_controller;
    Grounds_Controller     ground_controller;

    Sprite game_logo_sprite;
    Sprite play_btn_sprite;
    Sprite score_btn_sprite;
    Sprite selector_sprite;
    Sprite select_hint_sprite;
    Sprite confirm_hint_sprite;
    Sprite personal_credits_sprite;
    Sprite return_hint_sprite;

    Animation selector_anim;

    Vec2 screen_mid_bottom;
    Vec2 screen_mid_top;
    Vec2 screen_center;

    f32 selector_anim_elapsed;
    f32 delta;

    Sound wing_sfx;

    color bg_color;
    color black_color;
    int   current_option;
    int   options_count;
    int   load_step;
    bool  show_credits;
    float scale;

    Texture_Region play_btn_up;
    Texture_Region play_btn_down;
    Texture_Region score_btn_up;
    Texture_Region score_btn_down;
} Menu_Screen;

Screen_Desc menu_screen_init(Menu_Screen *s);
#endif

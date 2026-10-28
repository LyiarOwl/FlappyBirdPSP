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

    Animation selector_anim;

    Vec2 screen_mid_bottom;
    Vec2 screen_mid_top;
    Vec2 description_str_size;

    f32 selector_anim_elapsed;
    f32 delta;

    color bg_color;
    color black_color;
    int   current_option;
    int   options_count;
    int   load_step;

    Texture_Region play_btn_up;
    Texture_Region play_btn_down;
    Texture_Region score_btn_up;
    Texture_Region score_btn_down;
} Menu_Screen;

Screen_Desc menu_screen_init(Menu_Screen *s);
#endif

#ifndef GAME2_SCREEN_H
#define GAME2_SCREEN_H

#include "lframework/animation.h"
#include "lframework/camera2d.h"
#include "lframework/texture2d.h"
#include "phwind/phw_debug_renderer.h"
#include "pspgame/entities/backgrounds_controller.h"
#include "pspgame/entities/bird.h"
#include "pspgame/entities/grounds_controller.h"
#include "pspgame/entities/pipes_controller.h"
#include "pspgame/screen_manager.h"

typedef struct
{
    Screen_Desc desc;
    Texture2D   spritesheet;

    Camera2D camera;

    Backgrounds_Controller background_contrl;
    Grounds_Controller     ground_contrl;
    Pipes_Controller       pipes_contrl;
    Bird                   bird;

    Animation jump_hint_lower_anim;
    Sprite    jump_hint_upper_sprite;
    Sprite    jump_hint_lower_sprite;

    Phw_Debug_Renderer *debug_renderer;

    bool game_over;
    bool start;

    color clear_color;

    int load_step;

    float jump_hint_lower_elapsed;
    float foreground_scroll_speed;
    float scale;
    float delta;
} Game_Screen;

Screen_Desc game_screen_init(Game_Screen *s);

#endif

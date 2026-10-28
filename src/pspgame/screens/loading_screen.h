#ifndef LOADING_SCREEN_H
#define LOADING_SCREEN_H

#include "lframework/animation.h"
#include "lframework/sprite.h"
#include "lframework/texture2d.h"

#include "pspgame/screen_manager.h"

typedef struct
{
    Texture2D spritesheet;

    Animation bird_anim;
    Animation loading_text_anim;

    Sprite bird_sprite;
    Sprite loading_text_sprite;

    f32 bird_anim_elapsed, loading_text_elapsed;
    f32 delta;

    Screen_Desc desc;
} Loading_Screen;

Screen_Desc loading_screen_init(Loading_Screen *s);
#endif

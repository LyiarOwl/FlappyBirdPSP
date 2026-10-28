#ifndef INTRO_SCREEN_H
#define INTRO_SCREEN_H

#include "pspgame/screen_manager.h"
#include "lframework/texture2d.h"

typedef struct
{
    Texture2D   logo;
    float       alpha, timer, delta;
    int         phase;
    Screen_Desc desc;
} Intro_Screen;

void intro_screen_init(Intro_Screen *s);

#endif

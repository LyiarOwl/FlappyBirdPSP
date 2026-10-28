#ifndef GAME2_SCREEN_H
#define GAME2_SCREEN_H

#include "lframework/texture2d.h"
#include "pspgame/screen_manager.h"

typedef struct
{
    bool        game_over;
    bool        start;
    Texture2D   spritesheet;
    int         load_step;
    Screen_Desc desc;
} Game_Screen;

void game_screen_init(Game_Screen *s);

#endif

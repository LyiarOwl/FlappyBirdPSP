#ifndef GAME_SCREENS_H
#define GAME_SCREENS_H
#include "screen_manager.h"

typedef struct Game_Screens Game_Screens;

Game_Screens   *game_screens_new(void);

Screen_Manager *game_new_screens_manager(Game_Screens *game);

void            game_screens_free(Game_Screens *game);

#endif

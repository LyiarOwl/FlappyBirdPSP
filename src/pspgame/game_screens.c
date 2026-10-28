#include "game_screens.h"
#include "screens/game_screen.h"
#include "screens/intro_screen.h"
#include "screens/loading_screen.h"
#include "screens/menu_screen.h"
#include "screens/screen_ids.h"
#include <stdlib.h>

struct Game_Screens
{
    Screen_Manager *manager;
    Screen_Desc     desc[SCR_COUNT];
    Intro_Screen    intro;
    Loading_Screen  loading;
    Menu_Screen     menu;
    Game_Screen     game;
};

Game_Screens *game_screens_new()
{
    Game_Screens *g = calloc(1, sizeof(*g));

    if (!g)
        return NULL;

    intro_screen_init(&g->intro);
    loading_screen_init(&g->loading);
    menu_screen_init(&g->menu);
    game_screen_init(&g->game);

    g->desc[SCR_INTRO]   = g->intro.desc;
    g->desc[SCR_LOADING] = g->loading.desc;
    g->desc[SCR_MENU]    = g->menu.desc;
    g->desc[SCR_GAME]    = g->game.desc;
    g->manager           = screen_manager_new(g->desc, SCR_COUNT, SCR_INTRO, SCR_LOADING);

    if (!g->manager) {
        free(g);
        return NULL;
    }
    return g;
}

Screen_Manager *game_new_screens_manager(Game_Screens *g) { return g ? g->manager : NULL; }

void game_screens_free(Game_Screens *g)
{
    if (!g)
        return;
    screen_manager_free(g->manager);
    free(g);
}

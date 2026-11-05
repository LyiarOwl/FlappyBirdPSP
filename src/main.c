#include <pspctrl.h>
#include <pspdisplay.h>
#include <pspgu.h>
#include <pspmoduleinfo.h>
#include <pspsdk.h>
#include <pspsysmem.h>
#include <pspuser.h>
#include <stdbool.h>
#include <stddef.h>

#include "SDL3/SDL_audio.h"
#include "SDL3/SDL_error.h"
#include "SDL3/SDL_hints.h"
#include "SDL3/SDL_init.h"
#include "SDL3_mixer/SDL_mixer.h"
#include "lframework/bitmap_font.h"
#include "lframework/colors.h"
#include "lframework/common.h"
#include "lframework/sprite_batch.h"
#include "phwind/phw_world.h"
#include "pspgame/game_screens.h"
#include "pspgame/globals.h"
#include "pspgame/screen_manager.h"
#include "pspgame/screens/game_screen.h"
#include "pspgame/screens/loading_screen.h"
#include "pspgame/screens/menu_screen.h"
#include "psptypes.h"
#include "settings.h"

PSP_MODULE_INFO("PSPGame", 0, 1, 0);

char dispList[0x40000] __attribute__((aligned(64)));

void init(void)
{
    sceGuInit();
    void *fbp0 = guGetStaticVramBuffer(BUFFER_WIDTH, BUFFER_HEIGHT, GU_PSM_8888);
    void *fbp1 = guGetStaticVramBuffer(BUFFER_WIDTH, BUFFER_HEIGHT, GU_PSM_8888);

    sceGuStart(GU_DIRECT, dispList);
    sceGuDrawBuffer(GU_PSM_8888, fbp0, BUFFER_WIDTH);
    sceGuDispBuffer(SCREEN_WIDTH, SCREEN_HEIGHT, fbp1, BUFFER_WIDTH);

    sceGuOffset(2048 - (SCREEN_WIDTH / 2), 2048 - (SCREEN_HEIGHT / 2));
    sceGuViewport(2048, 2048, SCREEN_WIDTH, SCREEN_HEIGHT);
    sceGuEnable(GU_SCISSOR_TEST);
    sceGuDepthRange(65535, 0);
    sceGuScissor(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

    sceGuEnable(GU_BLEND);
    sceGuBlendFunc(GU_ADD, GU_SRC_ALPHA, GU_ONE_MINUS_SRC_ALPHA, 0, 0);
    sceGuTexMode(GU_PSM_8888, 0, 0, GU_FALSE);

    sceGuFinish();
    sceGuDisplay(GU_TRUE);
}

int main(void)
{
    init();

    SDL_SetHint(SDL_HINT_AUDIO_DEVICE_SAMPLE_FRAMES, "512");
    if (!SDL_Init(SDL_INIT_AUDIO)) {
        l_assert(false, "[main]: unable to init sdl audio");
        l_printf("%s", SDL_GetError());
        return 1;
    }

    if (!MIX_Init()) {
        l_assert(false, "[main]: unable to init sdl mixer");
        l_printf("%s", SDL_GetError());
        SDL_Quit();
        return 2;
    }

    g_mixer = MIX_CreateMixerDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, NULL);
    if (!g_mixer) {
        l_assert(false, "[main]: unable to init sdl mixer");
        l_printf("%s", SDL_GetError());
        MIX_Quit();
        SDL_Quit();
        return 3;
    }

    g_input        = input_new(true);
    g_sprite_batch = sprite_batch_new(2048UL);
    // clang-format off
    g_world = phw_world_create_ex(
        (Phw_Rect){ {0, 0}, {SCREEN_WIDTH, SCREEN_HEIGHT} },
        1,
        1,
        30,
        30,
        30,
        30
    );
    // clang-format on

    Game_Screens   *game    = game_screens_new();
    Screen_Manager *manager = game_new_screens_manager(game);

    // Menu_Screen data        = { 0 };
    // Screen_Desc menu        = menu_screen_init(&data);
    // bool        menu_loaded = false;

    // Loading_Screen data = { 0 };
    // Screen_Desc    load = loading_screen_init(&data);
    // load.enter(&data, NULL);

    // Game_Screen data        = { 0 };
    // Screen_Desc game        = game_screen_init(&data);
    // bool        game_loaded = false;
    // game.enter(&data, NULL);

    while (1) {
        // if (!menu_loaded) {
        //     Load_Result result = menu.load(&data, NULL);
        //     if (result == LOAD_DONE) {
        //         menu_loaded = true;
        //     }
        // }

        // if (!game_loaded) {
        //     Load_Result result = game.load(&data, NULL);
        //     if (result == LOAD_DONE) {
        //         game_loaded = true;
        //     }
        // }

        // update
        input_update(&g_input);
        screen_manager_update(manager);

        // if (menu_loaded)
        //     menu.update(&data, NULL);

        // if (game_loaded)
        //     game.update(&data, NULL);

        // load.update(&data, NULL);

        sceGuStart(GU_DIRECT, dispList);

        // draw
        // if (menu_loaded)
        //     menu.draw(&data, NULL);

        // if (game_loaded)
        //     game.draw(&data, NULL);

        screen_manager_draw(manager);

        // load.draw(&data, NULL);

        sceGuFinish();
        sceGuSync(GU_SYNC_FINISH, GU_SYNC_WHAT_DONE);
        sceDisplayWaitVblank();
        sceGuSwapBuffers();

        screen_manager_commit(manager);
    }

    return 0;
}

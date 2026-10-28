#include <pspctrl.h>
#include <pspdisplay.h>
#include <pspgu.h>
#include <pspmoduleinfo.h>
#include <pspsdk.h>
#include <pspsysmem.h>
#include <pspuser.h>
#include <stddef.h>

#include "lframework/bitmap_font.h"
#include "lframework/colors.h"
#include "lframework/common.h"
#include "lframework/sprite_batch.h"
#include "phwind/phw_world.h"
#include "pspgame/game_screens.h"
#include "pspgame/globals.h"
#include "pspgame/screen_manager.h"
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

    input        = input_new(true);
    sprite_batch = sprite_batch_new(2048UL);
    // clang-format off
    world = phw_world_create_ex(
        (Phw_Rect){ {0, 0}, {SCREEN_WIDTH, SCREEN_HEIGHT} },
        1,
        1,
        10,
        10,
        10,
        10
    );
    // clang-format on

    Game_Screens   *game    = game_screens_new();
    Screen_Manager *manager = game_new_screens_manager(game);

    // Menu_Screen data        = { 0 };
    // Screen_Desc menu        = menu_screen_init(&data);
    // bool        menu_loaded = false;

    Loading_Screen data = { 0 };
    Screen_Desc    load = loading_screen_init(&data);
    load.enter(&data, NULL);

    while (1) {
        // if (!menu_loaded) {
        //     Load_Result result = menu.load(&data, NULL);
        //     if (result == LOAD_DONE) {
        //         menu_loaded = true;
        //     }
        // }

        // update
        input_update(&input);
        screen_manager_update(manager);

        // if (menu_loaded)
        //     menu.update(&data, NULL);

        // load.update(&data, NULL);

        sceGuStart(GU_DIRECT, dispList);

        // draw
        // if (menu_loaded)
        //     menu.draw(&data, NULL);

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

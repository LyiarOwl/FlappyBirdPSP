#include "globals.h"
#include "SDL3_mixer/SDL_mixer.h"
#include "lframework/input.h"
#include "lframework/sprite_batch.h"
#include "phwind/phw_world.h"
#include "wchar.h"

Input         g_input        = { 0 };
Phw_World     g_world        = { 0 };
Sprite_Batch *g_sprite_batch = NULL;
MIX_Mixer    *g_mixer        = NULL;

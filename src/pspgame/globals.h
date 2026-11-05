#pragma once

#include "lframework/input.h"
#include "lframework/sprite_batch.h"
#include "phwind/phw_world.h"
#include <SDL3_mixer/SDL_mixer.h>

extern Input         g_input;
extern Sprite_Batch *g_sprite_batch;
extern Phw_World     g_world;
extern MIX_Mixer    *g_mixer;
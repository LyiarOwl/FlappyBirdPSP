#pragma once

#include "SDL3_mixer/SDL_mixer.h"
typedef struct sound
{
    MIX_Audio *audio;
    MIX_Track *track;
    bool       available;
} Sound;

Sound sound_new(MIX_Mixer *, const char *path);
void  sound_free(Sound *);

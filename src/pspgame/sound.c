#include "sound.h"
#include "SDL3_mixer/SDL_mixer.h"
#include "lframework/common.h"
#include <stdbool.h>

Sound sound_new(MIX_Mixer *mixer, const char *path)
{
    Sound s = { 0 };

    if (!mixer) {
        l_assert(false, "[sound_new]: mixer reference is null");
        return s;
    }

    if (!path) {
        l_assert(false, "[sound_new]: filepath is null");
        return s;
    }

    s.track = MIX_CreateTrack(mixer);
    s.audio = MIX_LoadAudio(mixer, path, false);

    MIX_SetTrackAudio(s.track, s.audio);
    MIX_SetTrackGain(s.track, 1.f);

    s.available = true;

    return s;
}

void sound_free(Sound *s)
{
    if (!s)
        return;

    MIX_DestroyTrack(s->track);
    MIX_DestroyAudio(s->audio);
    s->available = false;
}

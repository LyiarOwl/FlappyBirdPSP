#pragma once

#include "array.h"
#include "texture_region.h"

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include "types.h"
#include <stdbool.h>

#define anim_new animation_new
#define anim_get_kf_id animation_get_keyframe_index
#define anim_get_kf animation_get_keyframe
#define anim_get_kf_loop animation_get_keyframe_looping
#define anim_get_finished animation_is_animation_finished

typedef enum animation_play_mode {
    ANIM_PLAY_MODE_NORMAL,
    ANIM_PLAY_MODE_LOOP,
    ANIM_PLAY_MODE_COUNT
} Animation_Play_Mode;

typedef struct animation
{
    f32                 frame_duration;
    f32                 animation_duration;
    f32                 last_state_time;
    i32                 last_frame_number;
    Animation_Play_Mode play_mode;
    Array               frames_array;
} Animation;

Animation animation_new(f32 frame_duration, Texture_Region frames[], usize frames_length);

usize animation_get_keyframe_index(Animation *, f32 state_time);

const Texture_Region *animation_get_keyframe(Animation *, f32 state_time);

const Texture_Region *animation_get_keyframe_looping(Animation *, f32 state_time, bool looping);

bool animation_is_animation_finished(const Animation *, f32 state_time);

void animation_set_frame_duration(Animation *, f32 frame_duration);

void animation_free(Animation *);

void animation_set_play_mode(Animation *, Animation_Play_Mode play_mode);

#ifdef __cplusplus
}
#endif

#include "animation.h"
#include "array.h"
#include "common.h"
#include "texture_region.h"

#include <stdbool.h>
#include <stdlib.h>

Animation animation_new(f32 frame_duration, Texture_Region frames[], usize frames_length)
{
    Animation anim = { 0 };
    if (frame_duration <= 0.f) {
        l_assert(false, "[>>] animation_create: frame_duration must be >= 0.05");
        return anim;
    }

    if (!frames) {
        l_assert(false, "[>>] animation_create: frames[] reference is null!");
        return anim;
    }

    if (frames_length == 0) {
        l_assert(false, "[>>] animation_create: animation must have at least 1 frame!");
        return anim;
    }

    anim.frames_array = array_new(frames_length, sizeof(Texture_Region));

    for (usize i = 0; i < frames_length; i++) {
        array_set(&anim.frames_array, i, &frames[i]);
    }
    animation_set_frame_duration(&anim, frame_duration);

    return anim;
}

usize animation_get_keyframe_index(Animation *animation, float state_time)
{
    if (animation->frames_array.capacity == 1)
        return 0;

    if (animation->play_mode < 0 || animation->play_mode >= ANIM_PLAY_MODE_COUNT) {
        l_assert(
            false,
            "lfrAnimation_getKeyFrameIndex: something is wrong, what is this "
            "play_mode???"
        );
        return 0UL;
    }

    i32 frame_number = (i32)(state_time / animation->frame_duration);
    switch (animation->play_mode) {
    case ANIM_PLAY_MODE_NORMAL:
        frame_number = i32_min(animation->frames_array.capacity - 1, frame_number);
        break;
    case ANIM_PLAY_MODE_LOOP:
        frame_number = frame_number % animation->frames_array.capacity;
        break;
    default:
        break;
    }

    animation->last_frame_number = frame_number;
    animation->last_state_time   = state_time;

    return frame_number;
}

const Texture_Region *animation_get_keyframe(Animation *animation, f32 state_time)
{
    if (animation->frames_array.capacity == 0)
        return NULL;

    i32 frame_number = animation_get_keyframe_index(animation, state_time);

    return array_get(&animation->frames_array, frame_number);
}

const Texture_Region *
animation_get_keyframe_looping(Animation *animation, f32 state_time, bool looping)
{
    if (animation->frames_array.capacity == 0)
        return NULL;

    Animation_Play_Mode old_play_mode = animation->play_mode;

    if (looping) {
        animation->play_mode = ANIM_PLAY_MODE_LOOP;
    } else {
        animation->play_mode = ANIM_PLAY_MODE_NORMAL;
    }

    const Texture_Region *frame = animation_get_keyframe(animation, state_time);
    animation->play_mode        = old_play_mode;
    return frame;
}

bool animation_is_animation_finished(const Animation *animation, f32 state_time)
{
    i32 frame_number = (i32)(state_time / animation->frame_duration);
    return animation->frames_array.capacity - 1 < (usize)frame_number;
}

void animation_set_frame_duration(Animation *animation, f32 frame_duration)
{
    animation->frame_duration     = frame_duration;
    animation->animation_duration = frame_duration * animation->frames_array.capacity;
}

void animation_set_play_mode(Animation *animation, Animation_Play_Mode play_mode)
{
    if (play_mode < 0 || play_mode >= ANIM_PLAY_MODE_COUNT) {
        l_assert(false, "[>>] lfrAnimation_setplay_mode: what is this play_mode???");
        return;
    }

    animation->play_mode = play_mode;
}

void animation_free(Animation *animation)
{
    if (!animation)
        return;

    array_free(&animation->frames_array);
}

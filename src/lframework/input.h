#pragma once

#include "types.h"
#include "vec2.h"
#include <pspctrl.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct input
{
    SceCtrlData pad;
    Vec2        analog;
    u32         curr_btn_state;
    u32         prev_btn_state;
    bool        is_analog_enabled;
} Input;

Input input_new(bool enable_analog);
void  input_update(Input *);
/* @param btn use PSP_CTRL_<BTN> to get the correct button id. */
bool input_is_btn_down(const Input *, u32 btn);
/* @param btn use PSP_CTRL_<BTN> to get the correct button id. */
bool input_is_btn_up(const Input *, u32 btn);
/* @param btn use PSP_CTRL_<BTN> to get the correct button id. */
bool input_was_btn_pressed(const Input *, u32 btn);
/* @param btn use PSP_CTRL_<BTN> to get the correct button id. */
bool input_was_btn_released(const Input *, u32 btn);
void input_toggle_analogs(Input *, bool toggle);

#ifdef __cplusplus
}
#endif

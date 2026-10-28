#include "input.h"
#include "common.h"
#include <pspctrl.h>

Input input_new(bool enable_analog)
{
    Input input = (Input){ 0 };
    input_toggle_analogs(&input, enable_analog);
    return input;
}

void input_update(Input *input)
{
    if (!input) {
        l_assert(false, "[>] input_update: input reference is null!");
        return;
    }

    sceCtrlReadBufferPositive(&input->pad, 1);

    if (input->is_analog_enabled) {
        float inv127    = 1.f / 127.f;
        input->analog.x = ((float)input->pad.Lx - 128.f) * inv127;
        input->analog.y = ((float)input->pad.Ly - 128.f) * inv127;
        if (f32_abs(input->analog.x) < 0.01f)
            input->analog.x = 0.f;
        if (f32_abs(input->analog.y) < 0.01f)
            input->analog.y = 0.f;
    }

    input->prev_btn_state = input->curr_btn_state;
    input->curr_btn_state = input->pad.Buttons;
}

bool input_is_btn_down(const Input *input, u32 btn)
{
    if (!input) {
        l_assert(false, "[>] input_isKeyDown: input reference is null!");
        return false;
    }

    return has_any_flag(input->curr_btn_state, btn);
}

bool input_is_btn_up(const Input *input, u32 btn)
{
    if (!input) {
        l_assert(false, "[>] input_isKeyUp: input reference is null!");
        return false;
    }

    return !has_any_flag(input->curr_btn_state, btn);
}

bool input_was_btn_pressed(const Input *input, u32 btn)
{
    if (!input) {
        l_assert(false, "[>] input_wasKeyPressed: input reference is null!");
        return false;
    }

    return input_is_btn_down(input, btn) &&
           !has_any_flag(input->prev_btn_state, btn);
}

bool input_was_btn_released(const Input *input, u32 btn)
{
    if (!input) {
        l_assert(false, "[>] input_wasKeyReleased: input reference is null!");
        return false;
    }

    return input_is_btn_up(input, btn) &&
           has_any_flag(input->prev_btn_state, btn);
}

void input_toggle_analogs(Input *input, bool toggle)
{
    if (!input) {
        l_assert(false, "[>] input_toggleAnalogs: input reference is null!");
        return;
    }

    input->is_analog_enabled = toggle;

    if (toggle) {
        sceCtrlSetSamplingCycle(0);
        sceCtrlSetSamplingMode(PSP_CTRL_MODE_ANALOG);
        return;
    }

    sceCtrlSetSamplingMode(PSP_CTRL_MODE_DIGITAL);
}

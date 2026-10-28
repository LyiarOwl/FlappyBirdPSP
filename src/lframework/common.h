#pragma once

#include <assert.h>
#include <math.h>
#include <pspfpu.h>
#include <pspgu.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

#include "colors.h"
#include "ppsspp_emu_api.h"
#include "types.h"

#define UTIL_INLINE static inline __attribute__((unused))
#define UTIL static __attribute__((unused))

#define Math_PI 3.14159f
#define Math_Deg2Rad 0.01745f
#define Math_Rad2Deg 57.29577f

UTIL_INLINE f32 f32_floor(f32 value) { return pspFpuFloor(value); }

UTIL_INLINE f32 f32_ceil(f32 value) { return pspFpuCeil(value); }

UTIL_INLINE f32 f32_round(f32 value) { return pspFpuRound(value); }

UTIL_INLINE f32 f32_sqrt(f32 value) { return pspFpuSqrt(value); }

UTIL_INLINE f32 f32_sin(f32 value) { return pspFpuSin(value); }

UTIL_INLINE f32 f32_cos(f32 value) { return pspFpuCos(value); }

UTIL_INLINE f32 f32_atan2(f32 y, f32 x) { return atan2f(y, x); }

UTIL_INLINE f32 f32_abs(f32 value) { return pspFpuAbs(value); }

UTIL_INLINE f32 f32_sign(f32 value) { return pspFpuSignFloat(value); }

UTIL_INLINE f32 f32_max(f32 a, f32 b) { return pspFpuMax(a, b); }

UTIL_INLINE f32 f32_min(f32 a, f32 b) { return pspFpuMin(a, b); }

UTIL_INLINE f32 f32_clamp(f32 value, f32 min, f32 max)
{
    return f32_max(min, f32_min(value, max));
}

UTIL_INLINE f32 f32_lerp(f32 current, f32 target, f32 delta)
{
    return current + (target - current) * delta;
}

UTIL_INLINE f32 f32_mod(f32 a, f32 b) { return pspFpuTrunc(a / b) * b; }

UTIL_INLINE f32 f32_move_towards(f32 current, f32 target, f32 delta)
{
    return f32_abs(target - current) <= delta ? target
                                              : current + f32_sign(target - current) * delta;
}

UTIL_INLINE i32 i32_max(i32 a, i32 b)
{
    if (a >= b)
        return a;
    return b;
}

UTIL_INLINE i32 i32_min(i32 a, i32 b)
{
    if (a <= b)
        return a;
    return b;
}

UTIL_INLINE i32 i32_clamp(i32 value, i32 min, i32 max)
{
    return i32_max(min, i32_min(value, max));
}

UTIL_INLINE i32 i32_abs(i32 value)
{
    if (value >= 0)
        return value;
    return -value;
}

UTIL_INLINE i32 i32_sign(i32 value)
{
    if (value < 0)
        return -1;
    if (value > 0)
        return 1;
    return 0;
}

UTIL_INLINE bool has_any_flag(u32 value, u32 mask) { return (value & mask) != 0; }

UTIL_INLINE bool has_all_flags(u32 value, u32 mask) { return (value & mask) == mask; }

/**
 * Clear the current framebuffer.
 */
UTIL void clear(color color)
{
    sceGuClearDepth(0);
    sceGuClearColor(color);
    sceGuClear(GU_COLOR_BUFFER_BIT | GU_DEPTH_BUFFER_BIT);
}

UTIL void print(const char *str)
{
    if (ppsspp_is_emulator()) {
        ppsspp_send_output_str(str);
    }
}

UTIL void l_printf(const char *format, ...)
{
    char buffer[256];

    va_list args;
    va_start(args, format);

    vsnprintf(buffer, sizeof(buffer), format, args);

    va_end(args);

    print(buffer);
}

UTIL_INLINE void l_assert(bool expression, const char *message)
{
    if (!expression) {
        print(message);
        assert(false);
    }
}

#ifdef __cplusplus
}
#endif

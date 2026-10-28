#pragma once

#include "phw_types.h"
#include <assert.h>
#include <pspfpu.h>
#include <pspgu.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

#include "ppsspp_emu_api.h"

static inline f32 phw_ffloor(f32 value) { return pspFpuFloor(value); }

static inline f32 phw_fabs(f32 value) { return pspFpuAbs(value); }

static inline f32 phw_fsign(f32 value) { return pspFpuSignFloat(value); }

static inline f32 phw_fmax(f32 a, f32 b) { return pspFpuMax(a, b); }

static inline f32 phw_fmin(f32 a, f32 b) { return pspFpuMin(a, b); }

static inline f32 phw_fsqrt(f32 v) { return pspFpuSqrt(v); };

static inline f32 phw_fclamp(f32 value, f32 min, f32 max)
{
    return phw_fmax(min, phw_fmin(value, max));
}

static inline i32 phw_imax(i32 a, i32 b)
{
    if (a >= b)
        return a;
    return b;
}

static inline i32 phw_imin(i32 a, i32 b)
{
    if (a <= b)
        return a;
    return b;
}

static inline i32 phw_iclamp(i32 value, i32 min, i32 max)
{
    return phw_imax(min, phw_imin(value, max));
}

static inline i32 phw_iabs(i32 value)
{
    if (value >= 0)
        return value;
    return -value;
}

static inline i32 phw_isign(i32 value)
{
    if (value < 0)
        return -1;
    if (value > 0)
        return 1;
    return 0;
}

static inline bool phw_hasAnyFlag(uint32_t value, uint32_t mask)
{
    return (value & mask) != 0;
}

static inline bool phw_hasAllFlags(uint32_t value, uint32_t mask)
{
    return (value & mask) == mask;
}

static void phw_print(const char *str)
{
    if (ppsspp_is_emulator()) {
        ppsspp_send_output_str(str);
    }
}

static inline void phw_assert(bool expression, const char *message)
{
    if (!expression) {
        phw_print(message);
        assert(false);
    }
}

#ifdef __cplusplus
}
#endif

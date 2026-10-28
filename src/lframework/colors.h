#pragma once

#include "types.h"

#define UTIL_INLINE static inline __attribute__((unused))

#ifdef __cplusplus
extern "C" {
#endif

typedef u32 color;

#define COLOR_WHITE 0xFFFFFFFF
#define COLOR_BLACK 0xFF000000
#define COLOR_TRANSPARENT 0x00FFFFFF
#define COLOR_LIME 0xFF00FF00
#define COLOR_BLUE 0xFFFF0000
#define COLOR_RED 0xFF0000FF
#define COLOR_CORNFLOWERBLUE 0xFFed9564
#define COLOR_MONOGAMEORANGE 0xFF003CE7
#define COLOR_DARKBLUE 0xFF2D0700
#define COLOR_SLATEBLUE 0xcd5a69
#define COLOR_LIGHTPINK 0xFFc1b6ff
#define COLOR_GRAY 0xFF808080
#define COLOR_AQUA 0xFFffff00
#define COLOR_YELLOW 0xFF00FFFF

UTIL_INLINE color color_from_rgba(u8 r, u8 g, u8 b, u8 a)
{
    return ((u32)a << 24) | ((u32)b << 16) | ((u32)g << 8) | ((u32)r << 0);
}

UTIL_INLINE color color_set_alpha(color c, float alpha)
{
    alpha = alpha < 0.f ? 0.f : alpha > 1.f ? 1.f : alpha;

    u32 a = (u32)(alpha * 255.f + 0.5f);

    return (c & 0x00FFFFFFu) | (a << 24);
}

#ifdef __cplusplus
}
#endif

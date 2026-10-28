#pragma once

#include "phw_circle.h"
#include "phw_rect.h"
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

bool phw_rect_vs_rect(Phw_Rect rect1, Phw_Rect rect2);

bool phw_circle_vs_rect(Phw_Circle circle, Phw_Rect rect);

bool phw_circle_vs_circle(
    Phw_Circle circle1, Phw_Circle circle2
);

#ifdef __cplusplus
}
#endif

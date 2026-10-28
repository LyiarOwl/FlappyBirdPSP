#include "phw_collision.h"
#include "phw_common.h"
#include "phw_vec2.h"

bool phw_rect_vs_rect(Phw_Rect rect1, Phw_Rect rect2)
{
    float a_half_w = rect1.size.x * 0.5f;
    float a_half_h = rect1.size.y * 0.5f;
    float a_left   = rect1.position.x - a_half_w;
    float a_top    = rect1.position.y - a_half_h;
    float a_right  = rect1.position.x + a_half_w;
    float a_bottom = rect1.position.y + a_half_h;

    float b_half_w = rect2.size.x * 0.5f;
    float b_half_h = rect2.size.y * 0.5f;
    float b_left   = rect2.position.x - b_half_w;
    float b_top    = rect2.position.y - b_half_h;
    float b_right  = rect2.position.x + b_half_w;
    float b_bottom = rect2.position.y + b_half_h;

    return a_left < b_right && a_right > b_left && a_top < b_bottom &&
           a_bottom > b_top;
}

bool phw_circle_vs_rect(Phw_Circle circle, Phw_Rect rect)
{
    float half_w = rect.size.x * 0.5f;
    float half_h = rect.size.y * 0.5f;
    float left   = rect.position.x - half_w;
    float top    = rect.position.y - half_h;
    float right  = rect.position.x + half_w;
    float bottom = rect.position.y + half_h;

    float nearest_x    = phw_fclamp(circle.position.x, left, right);
    float nearest_y    = phw_fclamp(circle.position.y, top, bottom);
    float delta_x      = circle.position.x - nearest_x;
    float delta_y      = circle.position.y - nearest_y;
    float squared_dist = delta_x * delta_x + delta_y * delta_y;

    return squared_dist < circle.radius * circle.radius;
}

bool phw_circle_vs_circle(Phw_Circle circle1, Phw_Circle circle2)
{
    float delta_y      = circle1.position.y - circle2.position.y;
    float delta_x      = circle1.position.x - circle2.position.x;
    float squared_dist = delta_x * delta_x + delta_y * delta_y;
    float total_radius = circle1.radius + circle2.radius;
    return squared_dist < total_radius * total_radius;
}

#include "phw_utils.h"
#include "phw_common.h"

bool phw_point_inside_slope(
    Phw_Vec2 point, Phw_Rect slope_bounds, Phw_Slope_Angle angle
)
{
    if (slope_bounds.size.x <= 0.f || slope_bounds.size.y <= 0.f)
        return false;
    f32 left = slope_bounds.position.x - slope_bounds.size.x * .5f;
    f32 top = slope_bounds.position.y - slope_bounds.size.y * .5f;
    f32 x = (point.x - left) / slope_bounds.size.x;
    f32 y = (point.y - top) / slope_bounds.size.y;
    if (x < 0.f || x >= 1.f || y < 0.f || y >= 1.f) return false;
    switch (angle) {
    case PHW_SLOPE_ANGLE_UP_RIGHT: return x + y > 1.f;
    case PHW_SLOPE_ANGLE_UP_LEFT: return y > x;
    case PHW_SLOPE_ANGLE_DOWN_RIGHT: return x > y;
    case PHW_SLOPE_ANGLE_DOWN_LEFT: return x + y < 1.f;
    default: return false;
    }
}

Phw_Vec2 phw_get_circle_separation(Phw_Circle circle1, Phw_Circle circle2)
{
    circle1.position.x = phw_ffloor(circle1.position.x);
    circle1.position.y = phw_ffloor(circle1.position.y);
    circle1.radius     = phw_ffloor(circle1.radius);

    circle2.position.x = phw_ffloor(circle2.position.x);
    circle2.position.y = phw_ffloor(circle2.position.y);
    circle2.radius     = phw_ffloor(circle2.radius);

    float deltaX = circle1.position.x - circle2.position.x;
    float deltaY = circle1.position.y - circle2.position.y;

    float distSquared = deltaX * deltaX + deltaY * deltaY;
    float minDist     = circle1.radius + circle2.radius;

    if (distSquared >= minDist * minDist)
        return (Phw_Vec2){ 0 };

    if (distSquared <= PHW_EPSILON)
        return (Phw_Vec2){ minDist, 0.f };

    float dist    = phw_fsqrt(distSquared);
    float overlap = minDist - dist;

    float invDist = 1.f / dist;
    float normalX = deltaX * invDist;
    float normalY = deltaY * invDist;
    return (Phw_Vec2){ normalX * overlap, normalY * overlap };
}

Phw_Vec2 phw_get_circle_rect_separation(Phw_Circle circle, Phw_Rect rect)
{
    circle.position.x = phw_ffloor(circle.position.x);
    circle.position.y = phw_ffloor(circle.position.y);
    rect.position.x = phw_ffloor(rect.position.x);
    rect.position.y = phw_ffloor(rect.position.y);
    if (circle.radius <= 0.f || rect.size.x <= 0.f || rect.size.y <= 0.f)
        return (Phw_Vec2){ 0.f, 0.f };

    float left = rect.position.x - rect.size.x * .5f;
    float right = rect.position.x + rect.size.x * .5f;
    float top = rect.position.y - rect.size.y * .5f;
    float bottom = rect.position.y + rect.size.y * .5f;
    float nx = phw_fclamp(circle.position.x, left, right);
    float ny = phw_fclamp(circle.position.y, top, bottom);
    float dx = circle.position.x - nx;
    float dy = circle.position.y - ny;
    float dsq = dx * dx + dy * dy;
    if (dsq >= circle.radius * circle.radius)
        return (Phw_Vec2){ 0.f, 0.f };
    if (dsq > PHW_EPSILON) {
        float dist = phw_fsqrt(dsq);
        float penetration = (circle.radius - dist) / dist;
        return (Phw_Vec2){ dx * penetration, dy * penetration };
    }

    /* Circle center inside/on rectangle: choose shortest outward exit. */
    float dl = circle.position.x - left;
    float dr = right - circle.position.x;
    float dt = circle.position.y - top;
    float db = bottom - circle.position.y;
    float min = phw_fmin(phw_fmin(dl, dr), phw_fmin(dt, db));
    if (min == dl) return (Phw_Vec2){ -(circle.radius + dl), 0.f };
    if (min == dr) return (Phw_Vec2){ circle.radius + dr, 0.f };
    if (min == dt) return (Phw_Vec2){ 0.f, -(circle.radius + dt) };
    return (Phw_Vec2){ 0.f, circle.radius + db };
}

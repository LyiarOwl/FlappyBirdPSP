#include "camera2d.h"
#include "common.h"
#include "rectangle.h"
#include "vec2.h"
#include "settings.h"
#include <stdbool.h>

Camera2D camera2d_new(void)
{
    // clang-format off
    return (Camera2D){
        .x = 0.f,
        .y = 0.f,
        .offset_x = 0.f,
        .offset_y = 0.f,
        .zoom     = 1.f
    };
    // clang-format on
}

Vec2 camera2d_screen_to_world(const Camera2D *camera, Vec2 position)
{
    if (!camera) {
        l_assert(
            false, "[>] camera2d_screenToWorld: camera reference is null!"
        );
        return (Vec2){ 0 };
    }

    Vec2  coords;
    float inv_cam_zoom = 1.f / camera->zoom;
    coords.x =
        ((position.x - camera->offset_x) * inv_cam_zoom) + camera->x;
    coords.y =
        ((position.y - camera->offset_y) * inv_cam_zoom) + camera->y;
    return coords;
}

Vec2 camera2d_world_to_screen(const Camera2D *camera, Vec2 position)
{
    if (!camera) {
        l_assert(
            false, "[>] camera2d_worldToScreen: camera reference is null!"
        );
        return (Vec2){ 0 };
    }

    Vec2 coords;
    coords.x =
        ((position.x - camera->x) * camera->zoom) + camera->offset_x;
    coords.y =
        ((position.y - camera->y) * camera->zoom) + camera->offset_y;
    return coords;
}

void camera2d_clamp_to_bounds(Camera2D *camera, Rectangle bounds)
{
    if (!camera) {
        l_assert(
            false, "[>] camera2d_clampToBounds: camera reference is null!"
        );
        return;
    }

    const float inv_zoom = 1.f / camera->zoom;

    const float left   = camera->offset_x * inv_zoom;
    const float top    = camera->offset_y * inv_zoom;
    const float right  = (SCREEN_WIDTH - camera->offset_x) * inv_zoom;
    const float bottom = (SCREEN_HEIGHT - camera->offset_y) * inv_zoom;

    const float minX = bounds.x + left;
    const float maxX = bounds.x + bounds.w - right;

    const float minY = bounds.y + top;
    const float maxY = bounds.y + bounds.h - bottom;

    if (minX > maxX)
        camera->x = bounds.x + bounds.w * 0.5f;
    else
        camera->x = f32_clamp(camera->x, minX, maxX);

    if (minY > maxY)
        camera->y = bounds.y + bounds.h * 0.5f;
    else
        camera->y = f32_clamp(camera->y, minY, maxY);
}

#include "lframework/common.h"
#include "lframework/shape_renderer.h"

#include "phw_common.h"
#include "phw_debug_draw.h"
#include "phw_debug_renderer.h"
#include "phw_draw_requests.h"
#include "phw_vec2.h"
#include "settings.h"

#include <stdbool.h>
#include <stdlib.h>

#pragma region Draw routines
static void
draw_triangle(Shape_Renderer *renderer, Vec2 a, Vec2 b, Vec2 c, color color, float lineThickness)
{
    if (!renderer) {
        phw_assert(false, "[>] debugRenderer_drawTriangle: renderer reference is null!");
        return;
    }

    shape_renderer_line(renderer, a.x, a.y, b.x, b.y, color, lineThickness);
    shape_renderer_line(renderer, b.x, b.y, c.x, c.y, color, lineThickness);
    shape_renderer_line(renderer, c.x, c.y, a.x, a.y, color, lineThickness);
}

static void renderBodies(Phw_Debug_Draw_Body_Data data, Phw_User_Data userData)
{
    Phw_Debug_Renderer *usrData = (Phw_Debug_Renderer *)userData.data;

    color color_disabled = COLOR_GRAY;

    if (usrData->draw_bbox) {
        shape_renderer_stroke_rectangle(
            usrData->renderer,
            phw_bbox_get_left(&data.bbox),
            phw_bbox_get_top(&data.bbox),
            data.bbox.h_size.x * 2.f,
            data.bbox.h_size.y * 2.f,
            data.enabled ? usrData->bbox_color : color_disabled,
            usrData->line_thickness
        );
    }

    if (usrData->draw_bodies) {
        switch (data.collision_shape.type) {
        case PHW_COLL_SHAPE_CIRCLE:
            shape_renderer_stroke_arc(
                usrData->renderer,
                data.bbox.position.x,
                data.bbox.position.y,
                data.collision_shape.shape.radius,
                0.f,
                Math_PI * 2.f,
                data.enabled ? usrData->bodies_color : color_disabled,
                usrData->line_thickness,
                10
            );
            break;
        case PHW_COLL_SHAPE_RECT:
            shape_renderer_stroke_rectangle(
                usrData->renderer,
                phw_bbox_get_left(&data.bbox),
                phw_bbox_get_top(&data.bbox),
                data.collision_shape.shape.rect.h_size.x * 2.f,
                data.collision_shape.shape.rect.h_size.y * 2.f,

                data.enabled ? usrData->bodies_color : color_disabled,
                usrData->line_thickness
            );
            break;
        }
    }
}

static void renderTiles(Phw_Debug_Draw_Tile_Data data, Phw_User_Data userData)
{
    Phw_Debug_Renderer *usrData = (Phw_Debug_Renderer *)userData.data;

    if (!usrData->draw_tiles)
        return;

    if (data.tile.type == PHW_TILETYPE_EMPTY)
        return;

    switch (data.tile.type) {
    case PHW_TILETYPE_SOLID:
        shape_renderer_stroke_rectangle(
            usrData->renderer,
            data.position.x, data.position.y,
            data.tile_size, data.tile_size,
            usrData->tiles_color,
            usrData->line_thickness
        );
        break;
    case PHW_TILETYPE_SLOPE:
        switch (data.tile.angle) {
        case PHW_SLOPE_ANGLE_UP_RIGHT:
            draw_triangle(
                usrData->renderer,
                (Vec2){ data.position.x + data.tile_size, data.position.y },
                (Vec2){ data.position.x + data.tile_size, data.position.y + data.tile_size },
                (Vec2){ data.position.x, data.position.y + data.tile_size },
                usrData->tiles_color,
                usrData->line_thickness
            );
            break;

        case PHW_SLOPE_ANGLE_UP_LEFT:
            draw_triangle(
                usrData->renderer,
                (Vec2){ data.position.x, data.position.y },
                (Vec2){ data.position.x + data.tile_size, data.position.y + data.tile_size },
                (Vec2){ data.position.x, data.position.y + data.tile_size },
                usrData->tiles_color,
                usrData->line_thickness
            );
            break;

        case PHW_SLOPE_ANGLE_DOWN_RIGHT:
            draw_triangle(
                usrData->renderer,
                (Vec2){ data.position.x, data.position.y },
                (Vec2){ data.position.x + data.tile_size, data.position.y },
                (Vec2){ data.position.x + data.tile_size, data.position.y + data.tile_size },
                usrData->tiles_color,
                usrData->line_thickness
            );
            break;

        case PHW_SLOPE_ANGLE_DOWN_LEFT:
            draw_triangle(
                usrData->renderer,
                (Vec2){ data.position.x, data.position.y },
                (Vec2){ data.position.x + data.tile_size, data.position.y },
                (Vec2){ data.position.x, data.position.y + data.tile_size },
                usrData->tiles_color,
                usrData->line_thickness
            );
            break;
        }
        break;
    }
}

static void renderBucketsUnderBodies(Phw_Debug_Draw_Bucket_Data data, Phw_User_Data userData)
{
    Phw_Debug_Renderer *usrData = (Phw_Debug_Renderer *)userData.data;

    if (!usrData->draw_buckets_under_bodies)
        return;

    shape_renderer_stroke_rectangle(
        usrData->renderer,
        data.position.x, data.position.y,
        data.size.x, data.size.y,
        usrData->cells_under_bodies_color,
        usrData->line_thickness
    );
}

static void renderBucketsGrid(Phw_Debug_Draw_Bucket_Data data, Phw_User_Data userData)
{
    Phw_Debug_Renderer *usrData = (Phw_Debug_Renderer *)userData.data;

    if (!usrData->draw_buckets_grid)
        return;

    shape_renderer_stroke_rectangle(
        usrData->renderer,
        data.position.x, data.position.y,
        data.size.x, data.size.y,
        usrData->buckets_color,
        usrData->line_thickness
    );
}

static void processDrawRequests(Phw_Debug_Renderer *self)
{
    if (!self) {
        phw_assert(
            false,
            "[>] phw_DebugRenderer_processDraw_Requests: renderer reference is "
            "null!"
        );
        return;
    }

    if (!self->draw_requests_buffer)
        return;

    if (self->draw_requests_buffer->requests_count == 0)
        return;

    for (size_t i = 0; i < self->draw_requests_buffer->requests_count; i++) {
        const Phw_Draw_Request *request = &self->draw_requests_buffer->requests[i];

        switch (request->type) {
        case PHW_DRAW_REQUEST_TYPE_RECT:
            shape_renderer_stroke_rectangle(
                self->renderer,
                request->rect.position.x, request->rect.position.y,
                request->rect.size.x, request->rect.size.y,
                request->color,
                self->line_thickness
            );
            break;
        case PHW_DRAW_REQUEST_TYPE_CIRCLE:
            shape_renderer_stroke_arc(
                self->renderer,
                request->circle.position.x, request->circle.position.y,
                request->circle.radius,
                0.f,
                Math_PI * 2.f,
                request->color,
                self->line_thickness,
                10
            );
            break;
        case PHW_DRAW_REQUEST_TYPE_LINE:
            shape_renderer_line(
                self->renderer,
                request->line.start.x, request->line.start.y,
                request->line.end.x, request->line.end.y,
                request->color,
                self->line_thickness
            );
            break;
        }
    }

    phw_draw_request_clear_buffer(self->draw_requests_buffer);
}
#pragma endregion

Phw_Debug_Renderer *phw_debug_renderer_new(Phw_World *world, usize shape_renderer_vertices_limit)
{
    if (!world) {
        phw_assert(false, "[>] phw_DebugRendere_r_create: world reference is null!");
        return NULL;
    }

    Phw_Debug_Renderer *ptr = calloc(1, sizeof(Phw_Debug_Renderer));

    if (!ptr) {
        phw_assert(
            false,
            "[>] phw_DebugRendere_r_create: unable to allocate memory "
            "for the debug renderer!"
        );
        return NULL;
    }

    Shape_Renderer *rendererPtr = shape_renderer_new(shape_renderer_vertices_limit);

    if (!rendererPtr) {
        free(ptr);
        phw_assert(
            false,
            "[>] phw_DebugRendere_r_create: unable to allocate memory "
            "for the shape renderer!"
        );
        return NULL;
    }

    ptr->renderer = rendererPtr;
    ptr->world    = world;

    ptr->bbox_color               = COLOR_LIGHTPINK;
    ptr->bodies_color             = COLOR_LIME;
    ptr->buckets_color            = COLOR_GRAY;
    ptr->cells_under_bodies_color = COLOR_RED;
    ptr->tiles_color              = COLOR_AQUA;

    ptr->line_thickness = 1.f;

    ptr->draw_buckets_grid         = true;
    ptr->draw_buckets_under_bodies = true;
    ptr->draw_bbox                 = true;
    ptr->draw_bodies               = true;
    ptr->draw_tiles                = true;

    // clang-format off
    ptr->debug_draw = (Phw_Debug_Draw){
        .render_bodies               = renderBodies,
        .render_buckets_grid         = renderBucketsGrid,
        .render_buckets_under_bodies = renderBucketsUnderBodies,
        .render_tiles                = renderTiles,
        .user_data = (Phw_User_Data) {
            .hint      = NULL,
            .data      = ptr,
            .data_size = sizeof(size_t)
        }
    };
    // clang-format on

    return ptr;
}

void phw_debug_renderer_set_world(Phw_Debug_Renderer *debugRenderer, const Phw_World *world)
{
    if (!debugRenderer) {
        phw_assert(false, "[>] phw_DebugRenderer_set_Renderer: renderer reference is null!");
        return;
    }

    debugRenderer->world = world;
}

void phw_debug_renderer_draw(Phw_Debug_Renderer *debugRenderer, const Camera2D *camera)
{
    if (!debugRenderer) {
        phw_assert(false, "[>] phw_DebugRende_rer_draw: renderer reference is null!");
        return;
    }

    Vec2 min = (Vec2){ 0.f, 0.f };
    Vec2 max = (Vec2){ .x = SCREEN_WIDTH, .y = SCREEN_HEIGHT };
    if (camera) {
        min = camera2d_screen_to_world(camera, (Vec2){ 0 });
        max = camera2d_screen_to_world(camera, (Vec2){ SCREEN_WIDTH, SCREEN_HEIGHT });
    }
    Phw_Rect viewBounds = {
        (Phw_Vec2){ min.x, min.y },
        (Phw_Vec2){ max.x - min.x, max.y - min.y }
    };

    shape_renderer_begin(debugRenderer->renderer, camera);
    phw_debug_draw_process_data(&debugRenderer->debug_draw, debugRenderer->world, &viewBounds);
    processDrawRequests(debugRenderer);

    shape_renderer_end(debugRenderer->renderer);
}

void phw_debug_renderer_push_draw_request(
    Phw_Debug_Renderer *debugRenderer, Phw_Draw_Request request
)
{
    if (!debugRenderer) {
        phw_assert(
            false,
            "[>] phw_DebugRenderer_pushDraw_Requests: renderer reference is "
            "null!"
        );
        return;
    }

    if (!debugRenderer->draw_requests_buffer)
        debugRenderer->draw_requests_buffer = phw_draw_request_buffer_create(32);
    if (!debugRenderer->draw_requests_buffer)
        return;

    phw_draw_request_push_request(debugRenderer->draw_requests_buffer, request);
}

void phw_debug_renderer_free(Phw_Debug_Renderer *debugRenderer)
{
    if (!debugRenderer)
        return;

    shape_renderer_free(debugRenderer->renderer);
    if (debugRenderer->draw_requests_buffer) {
        phw_draw_request_destroy_buffer(debugRenderer->draw_requests_buffer);
    }
    free(debugRenderer);
}

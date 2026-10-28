#include "shape_renderer.h"
#include "common.h"
#include "vertex_color.h"

#include <malloc.h>
#include <pspgu.h>
#include <pspgum.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#define MIN_VERTICES_LIMIT 32

/* Early-return guard with a consistent assert message. */
#define SR_REQUIRE(cond, fn, msg)                                                                  \
    do {                                                                                           \
        if (!(cond)) {                                                                             \
            l_assert(false, "[>] " fn ": " msg);                                                   \
            return;                                                                                \
        }                                                                                          \
    } while (0)

/* Guard for every drawing function: valid renderer + begin() was called. */
#define SR_REQUIRE_BATCHING(renderer, fn)                                                          \
    SR_REQUIRE(renderer, fn, "shape renderer reference is null!");                                 \
    SR_REQUIRE((renderer)->is_batching, fn, "you must call shape_renderer_begin first!")

/* ------------------------------------------------------------------------ */
/* Internal helpers                                                         */
/* ------------------------------------------------------------------------ */

/* Flushes if fewer than `count` free vertices remain. count must be <= capacity. */
static inline void reserve(Shape_Renderer *renderer, usize count)
{
    if (renderer->vertex_count + count > renderer->vertices_length)
        shape_renderer_flush(renderer);
}

static inline void push_vertex(Shape_Renderer *renderer, color tint, f32 x, f32 y)
{
    renderer->vertices[renderer->vertex_count++] = (Vertex_Color){
        .color = tint, { .x = x, .y = y, .z = 0.f }
    };
}

/* Caller must have called reserve(renderer, 3). */
static inline void
push_triangle(Shape_Renderer *renderer, color tint, f32 x0, f32 y0, f32 x1, f32 y1, f32 x2, f32 y2)
{
    push_vertex(renderer, tint, x0, y0);
    push_vertex(renderer, tint, x1, y1);
    push_vertex(renderer, tint, x2, y2);
}

/*
 *   Caller must have called reserve(renderer, 6).
 *   Corners must be given in order around the quad (0 -> 1 -> 2 -> 3).
 */
static inline void push_quad(
    Shape_Renderer *renderer,
    color           tint,
    f32             x0,
    f32             y0,
    f32             x1,
    f32             y1,
    f32             x2,
    f32             y2,
    f32             x3,
    f32             y3
)
{
    push_triangle(renderer, tint, x0, y0, x1, y1, x3, y3);
    push_triangle(renderer, tint, x1, y1, x2, y2, x3, y3);
}

/* ------------------------------------------------------------------------ */
/* Lifecycle                                                                */
/* ------------------------------------------------------------------------ */

Shape_Renderer *shape_renderer_new(usize vertices_limit)
{
    if (vertices_limit < MIN_VERTICES_LIMIT)
        vertices_limit = MIN_VERTICES_LIMIT;

    Shape_Renderer *renderer =
        calloc(1, sizeof(Shape_Renderer) + vertices_limit * sizeof(Vertex_Color));

    if (!renderer) {
        l_assert(false, "[>] shape_renderer_new: unable to allocate memory for the shape renderer");
        return NULL;
    }

    renderer->vertices_length = vertices_limit;

    return renderer;
}

void shape_renderer_free(Shape_Renderer *renderer)
{
    if (!renderer)
        return;

    free(renderer);
}

/* ------------------------------------------------------------------------ */
/* Begin / flush / end                                                      */
/* ------------------------------------------------------------------------ */

void shape_renderer_begin(Shape_Renderer *renderer, const Camera2D *camera)
{
    SR_REQUIRE(renderer, "shape_renderer_begin", "shape renderer reference is null!");

    if (renderer->is_batching)
        return;

    renderer->is_batching = true;

    sceGuShadeModel(GU_FLAT);
    sceGuDisable(GU_CULL_FACE); // triangle winding depends on the shape/direction
    sceGuDisable(GU_TEXTURE_2D);

    sceGumMatrixMode(GU_PROJECTION);
    sceGumLoadIdentity();
    sceGumOrtho(0.f, 480.f, 272.f, 0.f, -1.f, 1.f);

    sceGumMatrixMode(GU_VIEW);
    sceGumLoadIdentity();
    if (camera) {
        ScePspFVector3 origin = { camera->offset_x, camera->offset_y, 0.f };
        sceGumTranslate(&origin);
        ScePspFVector3 scale = { camera->zoom, camera->zoom, 1.f };
        sceGumScale(&scale);
        ScePspFVector3 position = { -camera->x, -camera->y, 0.f };
        sceGumTranslate(&position);
    }

    sceGumMatrixMode(GU_MODEL);
    sceGumLoadIdentity();
}

void shape_renderer_flush(Shape_Renderer *renderer)
{
    SR_REQUIRE(renderer, "shape_renderer_flush", "shape renderer reference is null!");

    if (!renderer->is_batching || renderer->vertex_count == 0)
        return;

    /*
     *       The GE reads the vertices later, when the display list executes, so the
     *       buffer can't be handed over directly (it would be overwritten by the
     *       next flush in the same frame). Copy into display-list memory instead.
     */
    usize         bytes    = renderer->vertex_count * sizeof(Vertex_Color);
    Vertex_Color *vertices = sceGuGetMemory(bytes);
    memcpy(vertices, renderer->vertices, bytes);

    sceGumDrawArray(
        GU_TRIANGLES,
        GU_COLOR_8888 | GU_VERTEX_32BITF | GU_TRANSFORM_3D,
        renderer->vertex_count,
        0,
        vertices
    );

    renderer->vertex_count = 0;
    renderer->flush_count++;
}

void shape_renderer_end(Shape_Renderer *renderer)
{
    SR_REQUIRE(renderer, "shape_renderer_end", "shape renderer reference is null!");

    if (!renderer->is_batching)
        return;

    shape_renderer_flush(renderer);

    renderer->is_batching = false;
}

/* ------------------------------------------------------------------------ */
/* Drawing                                                                  */
/* ------------------------------------------------------------------------ */

void shape_renderer_line(
    Shape_Renderer *renderer,
    f32             start_x,
    f32             start_y,
    f32             end_x,
    f32             end_y,
    color           tint,
    f32             thickness
)
{
    SR_REQUIRE_BATCHING(renderer, "shape_renderer_line");

    if (thickness <= 0.f)
        return;

    f32 dx     = end_x - start_x;
    f32 dy     = end_y - start_y;
    f32 length = f32_sqrt(dx * dx + dy * dy);
    if (length <= 0.0001f)
        return;

    /* Half-thickness perpendicular to the line direction. */
    f32 scale    = (thickness * 0.5f) / length;
    f32 normal_x = -dy * scale;
    f32 normal_y = dx * scale;

    reserve(renderer, 6);

    push_quad(
        renderer,
        tint,
        start_x - normal_x,
        start_y - normal_y,
        end_x - normal_x,
        end_y - normal_y,
        end_x + normal_x,
        end_y + normal_y,
        start_x + normal_x,
        start_y + normal_y
    );
}

void shape_renderer_fill_rectangle(Shape_Renderer *renderer, f32 x, f32 y, f32 w, f32 h, color tint)
{
    SR_REQUIRE_BATCHING(renderer, "shape_renderer_fill_rectangle");

    if (w <= 0.f || h <= 0.f)
        return;

    reserve(renderer, 6);

    push_quad(renderer, tint, x, y, x + w, y, x + w, y + h, x, y + h);
}

void shape_renderer_stroke_rectangle(
    Shape_Renderer *renderer, f32 x, f32 y, f32 w, f32 h, color tint, f32 thickness
)
{
    SR_REQUIRE_BATCHING(renderer, "shape_renderer_stroke_rectangle");

    if (thickness <= 0.f || w <= 0.f || h <= 0.f)
        return;

    f32 side_h = h - thickness * 2.f; // left/right bars, between top and bottom

    // Top
    shape_renderer_fill_rectangle(renderer, x, y, w, thickness, tint);

    // Bottom
    shape_renderer_fill_rectangle(renderer, x, y + h - thickness, w, thickness, tint);

    // Left
    shape_renderer_fill_rectangle(renderer, x, y + thickness, thickness, side_h, tint);

    // Right
    shape_renderer_fill_rectangle(
        renderer, x + w - thickness, y + thickness, thickness, side_h, tint
    );
}

void shape_renderer_stroke_arc(
    Shape_Renderer *renderer,
    f32             x,
    f32             y,
    f32             radius,
    f32             start,
    f32             end,
    color           tint,
    f32             thickness,
    i16             segments
)
{
    SR_REQUIRE_BATCHING(renderer, "shape_renderer_stroke_arc");

    if (thickness <= 0.f || radius <= 0.f)
        return;

    if (segments < 3)
        segments = 3;

    f32 arc = end - start;
    if (arc == 0.f)
        return;

    f32 half_thickness = thickness * 0.5f;
    f32 outer_radius   = radius + half_thickness;
    f32 inner_radius   = radius - half_thickness;
    if (inner_radius < 0.f)
        inner_radius = 0.f;

    f32 increment = arc / (f32)segments;

    f32 cos0 = f32_cos(start);
    f32 sin0 = f32_sin(start);

    for (int i = 0; i < segments; i++) {
        f32 angle1 = start + increment * (f32)(i + 1);
        f32 cos1   = f32_cos(angle1);
        f32 sin1   = f32_sin(angle1);

        f32 outer0x = x + cos0 * outer_radius;
        f32 outer0y = y + sin0 * outer_radius;
        f32 inner0x = x + cos0 * inner_radius;
        f32 inner0y = y + sin0 * inner_radius;

        f32 outer1x = x + cos1 * outer_radius;
        f32 outer1y = y + sin1 * outer_radius;
        f32 inner1x = x + cos1 * inner_radius;
        f32 inner1y = y + sin1 * inner_radius;

        reserve(renderer, 6);

        push_triangle(renderer, tint, outer0x, outer0y, inner0x, inner0y, outer1x, outer1y);
        push_triangle(renderer, tint, inner0x, inner0y, inner1x, inner1y, outer1x, outer1y);

        cos0 = cos1;
        sin0 = sin1;
    }
}

void shape_renderer_fill_arc(
    Shape_Renderer *renderer, f32 x, f32 y, f32 radius, f32 start, f32 end, color tint, i16 segments
)
{
    SR_REQUIRE_BATCHING(renderer, "shape_renderer_fill_arc");

    if (radius <= 0.f)
        return;

    if (segments < 3)
        segments = 3;

    f32 arc = end - start;
    if (arc == 0.f)
        return;

    f32 increment = arc / (f32)segments;

    f32 cos0 = f32_cos(start);
    f32 sin0 = f32_sin(start);

    for (int i = 0; i < segments; i++) {
        f32 angle1 = start + increment * (f32)(i + 1);
        f32 cos1   = f32_cos(angle1);
        f32 sin1   = f32_sin(angle1);

        reserve(renderer, 3);

        push_triangle(
            renderer,
            tint,
            x,
            y,                 // center
            x + cos0 * radius,
            y + sin0 * radius, // current point
            x + cos1 * radius,
            y + sin1 * radius  // next point
        );

        cos0 = cos1;
        sin0 = sin1;
    }
}

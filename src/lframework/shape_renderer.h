#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "camera2d.h"
#include "colors.h"
#include "types.h"
#include "vertex_color.h"

#include <stdbool.h>

/**
 Everything is queued as triangles: a line or rectangle costs 6 vertices, an
 arc costs 6 (stroke) or 3 (fill) vertices per segment.
 */
typedef struct shape_renderer
{
    usize        vertex_count;
    bool         is_batching;
    u32          flush_count;
    usize        vertices_length;
    Vertex_Color vertices[];
} Shape_Renderer;

/**
 @param vertices_limit capacity in vertices (clamped to a minimum of 32).
 @return NULL on failure.
 */
Shape_Renderer *shape_renderer_new(usize vertices_limit);

void shape_renderer_free(Shape_Renderer *);

/**
 @param camera can be NULL.
 */
void shape_renderer_begin(Shape_Renderer *, const Camera2D *camera);

void shape_renderer_end(Shape_Renderer *);

/**
 Sends everything queued so far to the GU. Called automatically when the
 buffer is full; you rarely need to call it yourself.
 */
void shape_renderer_flush(Shape_Renderer *);

void shape_renderer_line(
    Shape_Renderer *, f32 start_x, f32 start_y, f32 end_x, f32 end_y, color tint, f32 thickness
);

void shape_renderer_fill_rectangle(Shape_Renderer *, f32 x, f32 y, f32 w, f32 h, color tint);

/**
 The stroke is drawn inside the rectangle bounds.
 */
void shape_renderer_stroke_rectangle(
    Shape_Renderer *, f32 x, f32 y, f32 w, f32 h, color tint, f32 thickness
);

/**
 @param start,end angles in RADIANS.
 @param segments  number of segments (minimum 3).
 */
void shape_renderer_stroke_arc(
    Shape_Renderer *,
    f32   x,
    f32   y,
    f32   radius,
    f32   start,
    f32   end,
    color tint,
    f32   thickness,
    i16   segments
);

/**
 @param start,end angles in RADIANS.
 @param segments  number of segments (minimum 3).
 */
void shape_renderer_fill_arc(
    Shape_Renderer *, f32 x, f32 y, f32 radius, f32 start, f32 end, color tint, i16 segments
);

#ifdef __cplusplus
}
#endif

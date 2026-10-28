#pragma once

#include "lframework/camera2d.h"
#ifdef __cplusplus
extern "C" {
#endif

#include "lframework/colors.h"
#include "lframework/shape_renderer.h"
#include "phw_debug_draw.h"
#include "phw_draw_requests.h"

/**
 * This structure is an extension for the PhWind that uses LFramework.
 * This renderer is PSP exclusively (not agnostic).
 */
typedef struct phw_debug_renderer
{
    const Phw_World         *world;
    Shape_Renderer          *renderer;
    Phw_Draw_Request_Buffer *draw_requests_buffer;

    color tiles_color;
    color buckets_color;
    color cells_under_bodies_color;
    color bodies_color;
    color bbox_color;

    Phw_Debug_Draw debug_draw;

    f32 line_thickness;

    bool draw_tiles;
    bool draw_buckets_grid;
    bool draw_buckets_under_bodies;
    bool draw_bodies;
    bool draw_bbox;
} Phw_Debug_Renderer;

Phw_Debug_Renderer *phw_debug_renderer_new(Phw_World *, usize shape_renderer_vertices_limit);

/**
    @param camera can be NULL.
*/
void phw_debug_renderer_draw(Phw_Debug_Renderer *, const Camera2D *);

void phw_debug_renderer_set_world(Phw_Debug_Renderer *, const Phw_World *);

void phw_debug_renderer_push_draw_request(
    Phw_Debug_Renderer *, Phw_Draw_Request request
);

void phw_debug_renderer_free(Phw_Debug_Renderer *);

#ifdef __cplusplus
}
#endif

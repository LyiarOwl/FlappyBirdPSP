#pragma once

#include "phw_circle.h"
#include "phw_rect.h"
#include "phw_types.h"
#include "phw_vec2.h"

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>

typedef u32 color;

typedef enum phw_draw_request_type {
    PHW_DRAW_REQUEST_TYPE_RECT,
    PHW_DRAW_REQUEST_TYPE_CIRCLE,
    PHW_DRAW_REQUEST_TYPE_LINE
} Phw_Draw_Request_Type;

typedef struct phw_draw_request
{
    Phw_Draw_Request_Type type;
    color                 color;
    union {
        Phw_Rect   rect;
        Phw_Circle circle;
        struct
        {
            Phw_Vec2 start;
            Phw_Vec2 end;
        } line;
    };
} Phw_Draw_Request;

typedef struct phw_draw_request_buffer
{
    usize            requests_count;
    usize            requests_length;
    Phw_Draw_Request requests[];
} Phw_Draw_Request_Buffer;

Phw_Draw_Request_Buffer *phw_draw_request_buffer_create(usize requests_limit);

Phw_Draw_Request phw_draw_request_create_rect_request(Phw_Rect rect, color color);

Phw_Draw_Request phw_draw_request_create_circle_request(Phw_Circle circle, color color);

Phw_Draw_Request phw_draw_request_create_line_request(Phw_Vec2 start, Phw_Vec2 end, color color);

void phw_draw_request_push_request(Phw_Draw_Request_Buffer *, Phw_Draw_Request request);

void phw_draw_request_clear_buffer(Phw_Draw_Request_Buffer *);

void phw_draw_request_destroy_buffer(Phw_Draw_Request_Buffer *);

#ifdef __cplusplus
}
#endif

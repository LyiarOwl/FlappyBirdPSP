#include "phw_common.h"
#include "phw_draw_requests.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>

Phw_Draw_Request_Buffer *phw_draw_request_buffer_create(usize requests_limit)
{
    if (requests_limit == 0)
        requests_limit = 5;

    if (requests_limit > (SIZE_MAX - sizeof(Phw_Draw_Request_Buffer)) /
                             sizeof(Phw_Draw_Request))
        return NULL;

    Phw_Draw_Request_Buffer *ptr = calloc(
        1,
        sizeof(Phw_Draw_Request_Buffer) +
            requests_limit * sizeof(Phw_Draw_Request)
    );

    if (!ptr) {
        phw_assert(
            false,
            "[>] phwDrawRequests_createDrawRequestsBuffer: unable to allocate "
            "memory "
            "for creating the requests draw buffer!"
        );
        return NULL;
    }

    ptr->requests_length = requests_limit;

    return ptr;
}

void phw_draw_request_push_request(
    Phw_Draw_Request_Buffer *requestBuffer, Phw_Draw_Request request
)
{
    if (!requestBuffer) {
        phw_assert(
            false, "[>] phwDrawRequests_pushDrawRequest: buffer is null!"
        );
        return;
    }

    if (requestBuffer->requests_count >= requestBuffer->requests_length) {
        phw_assert(
            false, "[>] phwDrawRequests_pushDrawRequest: buffer overflow!"
        );
        return;
    }

    requestBuffer->requests[requestBuffer->requests_count] = request;
    requestBuffer->requests_count++;
}

void phw_draw_request_clear_buffer(Phw_Draw_Request_Buffer *requestBuffer)
{
    if (!requestBuffer) {
        phw_assert(
            false,
            "[>] phwDrawRequests_clearDrawRequestsBuffer: buffer is null!"
        );
        return;
    }

    requestBuffer->requests_count = 0;
}

Phw_Draw_Request phw_draw_request_create_rect_request(Phw_Rect rect, color color)
{
    // clang-format off
    return (Phw_Draw_Request){
        PHW_DRAW_REQUEST_TYPE_RECT,
        color,
        .rect = rect
    };
    // clang-format on
}

Phw_Draw_Request phw_draw_request_create_circle_request(Phw_Circle circle, color color)
{
    // clang-format off
    return (Phw_Draw_Request){
        PHW_DRAW_REQUEST_TYPE_CIRCLE,
        color,
        .circle = circle
    };
    // clang-format on
}

Phw_Draw_Request
phw_draw_request_create_line_request(Phw_Vec2 start, Phw_Vec2 end, color color)
{
    // clang-format off
    return (Phw_Draw_Request){
        PHW_DRAW_REQUEST_TYPE_LINE,
        color,
        .line.start = start,
        .line.end = end
    };
    // clang-format on
}

void phw_draw_request_destroy_buffer(Phw_Draw_Request_Buffer *requestBuffer)
{
    if (!requestBuffer)
        return;

    free(requestBuffer);
}

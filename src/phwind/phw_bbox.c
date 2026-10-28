#include "phw_bbox.h"
#include "phw_common.h"
#include <stdbool.h>

f32 phw_bbox_get_left(const Phw_Bbox *bbox)
{
    if (!bbox) {
        phw_assert(false, "[>] phwBbox_getLeft: bbox reference is null!");
        return 0.f;
    }

    float halfWidth = bbox->h_size.x;
    return bbox->position.x - halfWidth;
}

f32 phw_bbox_get_top(const Phw_Bbox *bbox)
{
    if (!bbox) {
        phw_assert(false, "[>] phwBbox_getTop: bbox reference is null!");
        return 0.f;
    }

    float halfHeight = bbox->h_size.y;
    return bbox->position.y - halfHeight;
}

f32 phw_bbox_get_right(const Phw_Bbox *bbox)
{
    if (!bbox) {
        phw_assert(false, "[>] phwBbox_getRight: bbox reference is null!");
        return 0.f;
    }

    float halfWidth = bbox->h_size.x;
    return bbox->position.x + halfWidth;
}

f32 phw_bbox_get_bottom(const Phw_Bbox *bbox)
{
    if (!bbox) {
        phw_assert(false, "[>] phwBbox_getBottom: bbox reference is null!");
        return 0.f;
    }

    float halfHeight = bbox->h_size.y;
    return bbox->position.y + halfHeight;
}

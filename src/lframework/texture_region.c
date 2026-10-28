#include "texture_region.h"
#include "common.h"
#include "rectangle.h"

Texture_Region texture_region_create(const Texture2D *texture, f32 x, f32 y, f32 w, f32 h)
{
    if (!texture) {
        l_assert(false, "[>] lfrTextureRegion_create: texture reference is null!");
        return (Texture_Region){ 0 };
    }

    // clang-format off
    return (Texture_Region){
        .texture = texture,
        .clip_rect = (Rectangle){x, y, w, h}
    };
    // clang-format on
}

Texture_Region
texture_region_create_sub_region(const Texture_Region *region, f32 x, f32 y, f32 w, f32 h)
{
    if (!region) {
        l_assert(false, "[>] lfrTextureRegion_createSubRegion: region reference is null!");
        return (Texture_Region){ 0 };
    }

    // clang-format off
    return (Texture_Region){
        .texture  = region->texture,
        .clip_rect = (Rectangle){
            region->clip_rect.x + x,
            region->clip_rect.y + y,
            w,
            h
        }
    };
    // clang-format on
}

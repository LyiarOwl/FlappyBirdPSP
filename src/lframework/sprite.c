#include "sprite.h"
#include "colors.h"
#include "common.h"
#include "sprite_batch.h"

Sprite sprite_new(Texture_Region region)
{
    // clang-format off
    Sprite sprite = {0};
    sprite.region = region;
    sprite.scale_x = 1.f;
    sprite.scale_y = 1.f;
    sprite.color = COLOR_WHITE;
    // clang-format on
    return sprite;
}

void sprite_set_origin_norm(Sprite *sprite, f32 x, f32 y)
{
    if (!sprite) {
        l_assert(false, "[>] Sprite_setOriginNormalized: sprite reference is null!");
        return;
    }

    sprite->origin_x = x * sprite->region.clip_rect.w;
    sprite->origin_y = y * sprite->region.clip_rect.h;
}

void sprite_draw(Sprite *sprite, Sprite_Batch *batch)
{
    if (!sprite) {
        l_assert(false, "[>] Sprite_draw: sprite reference is null!");
        return;
    }

    if (!batch) {
        l_assert(false, "[>] Sprite_draw: spritebatch reference is null!");
        return;
    }

    sprite_batch_draw_tex_pro(
        batch,
        sprite->region.texture,
        sprite->x,
        sprite->y,
        sprite->color,
        sprite->region.clip_rect.x,
        sprite->region.clip_rect.y,
        sprite->region.clip_rect.w,
        sprite->region.clip_rect.h,
        sprite->angle,
        sprite->origin_x,
        sprite->origin_y,
        sprite->scale_x,
        sprite->scale_y,
        sprite->flipX,
        sprite->flipY
    );
}

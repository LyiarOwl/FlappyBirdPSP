#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "bitmap_font.h"
#include "camera2d.h"
#include "colors.h"
#include "texture2d.h"
#include "types.h"
#include "vertex_texture_color.h"

#include <stdbool.h>

/**
 Every sprite is queued as two triangles (6 vertices), because GU_SPRITES
 can't be rotated. Size the batch with this, e.g.:

 sprite_batch_new(MAX_SPRITES * SPRITE_BATCH_VERTICES_PER_SPRITE);
 */
#define SPRITE_BATCH_VERTICES_PER_SPRITE 6

typedef struct sprite_batch
{
    const Texture2D      *current_texture;
    usize                 vertex_count;
    bool                  is_batching;
    u32                   flush_count;
    usize                 vertices_length;
    Vertex_Texture_Color *vertices;
} Sprite_Batch;

/**
 @param vertices_limit capacity in vertices (must be >=SPRITE_BATCH_VERTICES_PER_SPRITE).
 @return NULL on failure.
 */
Sprite_Batch *sprite_batch_new(usize vertices_limit);

void sprite_batch_free(Sprite_Batch *);

/**
 @param camera can be NULL.
 */
void sprite_batch_begin(Sprite_Batch *, const Camera2D *camera);

void sprite_batch_end(Sprite_Batch *);

/**
 Sends everything queued so far to the GU. Called automatically when the
 batch is full or the texture changes; you rarely need to call it yourself.
 */
void sprite_batch_flush(Sprite_Batch *);

/**
 Draws the whole texture with its top-left corner at (x, y).
 */
void sprite_batch_draw_tex(Sprite_Batch *, const Texture2D *, f32 x, f32 y, color tint);

/**
 @param clip_*   source rectangle inside the texture, in pixels.
 @param angle    rotation in DEGREES, clockwise on screen. |angle| <= 0.1
 takes a fast path without sin/cos.
 @param origin_* pivot, relative to the (unscaled) source rectangle. The
 sprite is placed so that its origin sits at (x, y), and
 it rotates around that point.
 @param scale_*  the sign is ignored; use flip_x / flip_y to mirror.
 */
void sprite_batch_draw_tex_pro(
    Sprite_Batch *,
    const Texture2D *,
    f32   x,
    f32   y,
    color tint,
    f32   clip_x,
    f32   clip_y,
    f32   clip_w,
    f32   clip_h,
    f32   angle,
    f32   origin_x,
    f32   origin_y,
    f32   scale_x,
    f32   scale_y,
    bool  flip_x,
    bool  flip_y
);

void sprite_batch_draw_str(
    Sprite_Batch *,
    const Bitmap_Font *,
    const char *text,
    f32         x,
    f32         y,
    f32         scale_x,
    f32         scale_y,
    color       tint
);

#ifdef __cplusplus
}
#endif

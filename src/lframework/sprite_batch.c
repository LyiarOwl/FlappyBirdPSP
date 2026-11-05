#include "sprite_batch.h"
#include "colors.h"
#include "common.h"
#include "vertex_texture_color.h"

#include <malloc.h>
#include <math.h>
#include <pspgu.h>
#include <pspgum.h>
#include <psptypes.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#define DEG2RAD 0.01745329252f
#define ROTATION_EPSILON 0.1f // degrees; below this, no sin/cos

/* Early-return guard with a consistent assert message. */
#define SB_REQUIRE(cond, fn, msg)                                                                  \
    do {                                                                                           \
        if (!(cond)) {                                                                             \
            l_assert(false, "[>] " fn ": " msg);                                                   \
            return;                                                                                \
        }                                                                                          \
    } while (0)

/* ------------------------------------------------------------------------ */
/* Internal helpers                                                         */
/* ------------------------------------------------------------------------ */

static inline void push_vertex(Sprite_Batch *batch, f32 u, f32 v, color tint, f32 x, f32 y)
{
    batch->vertices[batch->vertex_count++] = (Vertex_Texture_Color){
        { u, v },
        tint, { x, y, 0.f }
    };
}

/*
 *   Queues one quad as two triangles.
 *   Corner order: 0 = top-left, 1 = top-right, 2 = bottom-right, 3 = bottom-left.
 */
static void push_quad(
    Sprite_Batch *batch,
    color         tint,
    const f32     px[4],
    const f32     py[4],
    f32           u0,
    f32           v0,
    f32           u1,
    f32           v1
)
{
    push_vertex(batch, u0, v0, tint, px[0], py[0]);
    push_vertex(batch, u1, v0, tint, px[1], py[1]);
    push_vertex(batch, u1, v1, tint, px[2], py[2]);

    push_vertex(batch, u0, v0, tint, px[0], py[0]);
    push_vertex(batch, u1, v1, tint, px[2], py[2]);
    push_vertex(batch, u0, v1, tint, px[3], py[3]);
}

/* Makes room for one more sprite using `texture`, flushing if needed. */
static void reserve_sprite(Sprite_Batch *batch, const Texture2D *texture)
{
    bool texture_changed = batch->current_texture && batch->current_texture != texture;
    bool is_full = batch->vertex_count + SPRITE_BATCH_VERTICES_PER_SPRITE > batch->vertices_length;

    if (texture_changed || is_full)
        sprite_batch_flush(batch);

    batch->current_texture = texture;
}

/* ------------------------------------------------------------------------ */
/* Lifecycle                                                                */
/* ------------------------------------------------------------------------ */

Sprite_Batch *sprite_batch_new(usize vertices_limit)
{
    if (vertices_limit < SPRITE_BATCH_VERTICES_PER_SPRITE) {
        l_assert(
            false,
            "[>] sprite_batch_new: vertices_limit must be >= "
            "SPRITE_BATCH_VERTICES_PER_SPRITE"
        );
        return NULL;
    }

    Sprite_Batch *batch = calloc(1, sizeof(Sprite_Batch));
    if (!batch) {
        l_assert(false, "[>] sprite_batch_new: unable to allocate memory for the spritebatch");
        return NULL;
    }

    usize bytes = vertices_limit * sizeof(Vertex_Texture_Color);

    batch->vertices = memalign(16, bytes);
    if (!batch->vertices) {
        l_assert(false, "[>] sprite_batch_new: unable to allocate the vertex buffer");
        free(batch);
        return NULL;
    }

    memset(batch->vertices, 0, bytes);
    batch->vertices_length = vertices_limit;

    return batch;
}

void sprite_batch_free(Sprite_Batch *batch)
{
    if (!batch)
        return;

    free(batch->vertices);
    free(batch);
}

/* ------------------------------------------------------------------------ */
/* Begin / flush / end                                                      */
/* ------------------------------------------------------------------------ */

void sprite_batch_begin(Sprite_Batch *batch, const Camera2D *camera)
{
    SB_REQUIRE(batch, "sprite_batch_begin", "spritebatch reference is null!");

    if (batch->is_batching)
        return;

    batch->is_batching = true;

    sceGuShadeModel(GU_FLAT);
    sceGuDisable(GU_CULL_FACE); // flips and rotation change the triangle winding
    sceGuEnable(GU_TEXTURE_2D);
    sceGuTexFunc(GU_TFX_MODULATE, GU_TCC_RGBA);
    sceGuTexFilter(GU_NEAREST, GU_NEAREST);

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

void sprite_batch_flush(Sprite_Batch *batch)
{
    SB_REQUIRE(batch, "sprite_batch_flush", "spritebatch reference is null!");

    if (!batch->is_batching || batch->vertex_count == 0 || !batch->current_texture)
        return;

    sceGuTexImage(
        0,
        batch->current_texture->w,
        batch->current_texture->h,
        batch->current_texture->w,
        batch->current_texture->data
    );

    /*
     *       The GE reads the vertices later, when the display list executes, so the
     *       batch buffer can't be handed over directly (it gets overwritten by the
     *       next flush in the same frame). Copy into display-list memory instead.
     */
    usize                 bytes    = batch->vertex_count * sizeof(Vertex_Texture_Color);
    Vertex_Texture_Color *vertices = sceGuGetMemory(bytes);
    memcpy(vertices, batch->vertices, bytes);

    sceGumDrawArray(
        GU_TRIANGLES,
        GU_COLOR_8888 | GU_TEXTURE_32BITF | GU_VERTEX_32BITF | GU_TRANSFORM_3D,
        batch->vertex_count,
        0,
        vertices
    );

    batch->vertex_count = 0;
    batch->flush_count++;
}

void sprite_batch_end(Sprite_Batch *batch)
{
    SB_REQUIRE(batch, "sprite_batch_end", "spritebatch reference is null!");

    if (!batch->is_batching)
        return;

    sprite_batch_flush(batch);

    batch->is_batching     = false;
    batch->current_texture = NULL;
}

/* ------------------------------------------------------------------------ */
/* Drawing                                                                  */
/* ------------------------------------------------------------------------ */

void sprite_batch_draw_tex(Sprite_Batch *batch, const Texture2D *texture, f32 x, f32 y, color tint)
{
    SB_REQUIRE(texture, "sprite_batch_draw_tex", "texture reference is null!");

    sprite_batch_draw_tex_pro(
        batch,
        texture,
        x,
        y,
        tint,
        0.f,
        0.f,
        (f32)texture->w,
        (f32)texture->h,
        0.f,
        0.f,
        0.f,
        1.f,
        1.f,
        false,
        false
    );
}

void sprite_batch_draw_tex_pro(
    Sprite_Batch    *batch,
    const Texture2D *texture,
    f32              x,
    f32              y,
    color            tint,
    f32              clip_x,
    f32              clip_y,
    f32              clip_w,
    f32              clip_h,
    f32              angle,
    f32              origin_x,
    f32              origin_y,
    f32              scale_x,
    f32              scale_y,
    bool             flip_x,
    bool             flip_y
)
{
    SB_REQUIRE(batch, "sprite_batch_draw_tex_pro", "spritebatch reference is null!");
    SB_REQUIRE(
        batch->is_batching, "sprite_batch_draw_tex_pro", "you must call sprite_batch_begin first!"
    );
    SB_REQUIRE(texture, "sprite_batch_draw_tex_pro", "texture reference is null!");

    reserve_sprite(batch, texture);

    /* Texture coordinates (normalized, pixel-snapped source rect). */
    f32 inv_tex_width  = 1.f / texture->w;
    f32 inv_tex_height = 1.f / texture->h;
    clip_x             = f32_floor(clip_x);
    clip_y             = f32_floor(clip_y);

    f32 u0 = clip_x * inv_tex_width;
    f32 v0 = clip_y * inv_tex_height;
    f32 u1 = (clip_x + clip_w) * inv_tex_width;
    f32 v1 = (clip_y + clip_h) * inv_tex_height;

    if (flip_x) {
        f32 tmp = u0;
        u0      = u1;
        u1      = tmp;
    }

    if (flip_y) {
        f32 tmp = v0;
        v0      = v1;
        v1      = tmp;
    }

    /* Corners relative to the origin (x, y), before rotation. */
    f32 abs_scale_x = f32_abs(scale_x);
    f32 abs_scale_y = f32_abs(scale_y);

    f32 left   = -origin_x * abs_scale_x;
    f32 top    = -origin_y * abs_scale_y;
    f32 right  = (clip_w - origin_x) * abs_scale_x;
    f32 bottom = (clip_h - origin_y) * abs_scale_y;

    const f32 cx[4] = { left, right, right, left };
    const f32 cy[4] = { top, top, bottom, bottom };

    f32 px[4];
    f32 py[4];

    if (f32_abs(angle) <= ROTATION_EPSILON) {
        /* Fast path: axis-aligned, no sin/cos. */
        for (int i = 0; i < 4; i++) {
            px[i] = x + cx[i];
            py[i] = y + cy[i];
        }
    } else {
        f32 rad = angle * DEG2RAD;
        f32 s   = sinf(rad);
        f32 c   = cosf(rad);

        for (int i = 0; i < 4; i++) {
            px[i] = x + (cx[i] * c - cy[i] * s);
            py[i] = y + (cx[i] * s + cy[i] * c);
        }
    }

    push_quad(batch, tint, px, py, u0, v0, u1, v1);
}

void sprite_batch_draw_str(
    Sprite_Batch      *batch,
    const Bitmap_Font *font,
    const char        *text,
    f32                x,
    f32                y,
    f32                scale_x,
    f32                scale_y,
    color              tint
)
{
    SB_REQUIRE(batch, "sprite_batch_draw_str", "spritebatch reference is null!");
    SB_REQUIRE(
        batch->is_batching, "sprite_batch_draw_str", "you must call sprite_batch_begin first!"
    );
    SB_REQUIRE(font, "sprite_batch_draw_str", "font reference is null!");
    SB_REQUIRE(text, "sprite_batch_draw_str", "text is null!");

    f32 cursor_x = x;
    f32 cursor_y = y;

    for (const char *p = text; *p; p++) {
        char c = *p;

        if (c == '\n') {
            cursor_x = x;
            cursor_y += font->char_height * scale_y;
            continue;
        }

        /* Ignore any other control character. */
        if (c < 32)
            continue;

        int ascii_index = c - 32;
        int col         = ascii_index % font->chars_per_row;
        int row         = ascii_index / font->chars_per_row;

        sprite_batch_draw_tex_pro(
            batch,
            &font->texture,
            cursor_x,
            cursor_y,
            tint,
            (f32)(col * font->char_width),
            (f32)(row * font->char_height),
            (f32)font->char_width,
            (f32)font->char_height,
            0.f,
            0.f,
            0.f,
            scale_x,
            scale_y,
            false,
            false
        );

        cursor_x += (font->char_width + font->letter_spacing) * scale_x;
    }
}

#pragma once

#include "lframework/vec2.h"
#include "types.h"
#ifdef __cplusplus
extern "C" {
#endif

#include "texture2d.h"

typedef struct bitmap_font
{
    Texture2D texture;
    /* width of one character (default: 16) */
    i16 char_width;
    /* height of one character (default: 16) */
    i16 char_height;
    /* characters per row in the bitmap font (default: 16) */
    i16 chars_per_row;
    /* space between characters (default: -8) */
    i16 letter_spacing;
} Bitmap_Font;

Bitmap_Font
bitmap_font_load(const char *filepath, i16 char_width, i16 char_height, i16 letter_spacing);

/**
 * @brief Formats an integer array into a text buffer.
 *
 * Converts the values of an integer array into a formatted text string,
 * separating values with spaces and inserting a line break after the
 * specified number of columns.
 *
 * This can be useful for displaying arrays, matrices, or other integer
 * data using Bitmap_Font and sprite_batch_draw_text.
 *
 * @param text_buffer        Buffer where the formatted text will be written.
 * @param text_buffer_length Size of the text buffer in bytes.
 * @param array              Integer array to format.
 * @param array_length       Number of elements in the array.
 * @param columns            Number of values to write per line.
 *
 * @return Number of characters written to the buffer, excluding the null
 *         terminator.
 *
 * @example
 * int32_t values[] = {
 *     10, 20, 30,
 *     40, 50, 60,
 *     70, 80, 90
 * };
 *
 * char buffer[128];
 *
 * bitmap_font_set_text_buffer_to_draw_array_i32(
 *     buffer,
 *     sizeof(buffer),
 *     values,
 *     9,
 *     3
 * );
 *
 * sprite_batch_draw_text(
 *     spr_batch,
 *     &bitmap_font,
 *     buffer,
 *     10.f,
 *     10.f,
 *     COLOR_WHITE
 * );
 *
 * // Result:
 * // 10 20 30
 * // 40 50 60
 * // 70 80 90
 */
i32 bitmap_font_set_text_buffer_to_draw_array_i32(
    char *text_buffer, usize text_buffer_length, const int array[], usize array_length, i32 columns
);

Vec2 bitmap_font_measure_str(
    const Bitmap_Font *font, const char *text, float scale_x, float scale_y
);

void bitmap_font_unload(Bitmap_Font *);

#ifdef __cplusplus
}
#endif

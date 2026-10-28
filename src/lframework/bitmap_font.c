#include "bitmap_font.h"
#include "common.h"
#include "texture2d.h"
#include "types.h"

#include <pspgu.h>
#include <stdio.h>

Bitmap_Font
bitmap_font_load(const char *filepath, i16 char_width, i16 char_height, i16 letter_spacing)
{
    if (!filepath) {
        l_assert(false, "[>] bitmap_font_load: you forgot the path bitch");
        return (Bitmap_Font){ 0 };
    }

    // clang-format off
    return (Bitmap_Font){
        .texture       = texture2d_load(filepath),
        .char_width     = char_width,
        .char_height    = char_height,
        .chars_per_row   = 16,
        .letter_spacing = letter_spacing
    };
    // clang-format on
}

i32 bitmap_font_set_text_buffer_to_draw_array_i32(
    char *text_buffer, usize text_buffer_length, const int array[], usize array_length, i32 columns
)
{

    i32 offset = 0;

    for (usize i = 0; i < array_length; i++) {
        usize written =
            snprintf(text_buffer + offset, text_buffer_length - offset, "%d ", array[i]);

        if (written < 0 || written > text_buffer_length - offset)
            break;

        offset += written;

        if ((i + 1) % columns == 0) {
            written = snprintf(text_buffer + offset, text_buffer_length - offset, "\n");

            if (written < 0 || written > text_buffer_length - offset)
                break;

            offset += written;
        }
    }

    return offset;
}

Vec2 bitmap_font_measure_str(const Bitmap_Font *font, const char *text)
{
    if (!font || !text)
        return (Vec2){ 0 };

    f32   max_width   = 0.f;
    f32   line_width  = 0.f;
    f32   line_height = (f32)font->char_height;
    f32   advance     = (f32)font->char_width + font->letter_spacing;
    usize chars       = 0;
    usize lines       = 1;

    for (const unsigned char *p = (const unsigned char *)text; *p; p++) {
        if (*p == '\n') {
            if (chars > 0) {
                line_width = chars * advance - font->letter_spacing;
                if (line_width > max_width)
                    max_width = line_width;
            }

            chars = 0;
            lines++;
            continue;
        }

        if (*p < 32)
            continue;

        chars++;
    }

    if (chars > 0) {
        line_width = chars * advance - font->letter_spacing;
        if (line_width > max_width)
            max_width = line_width;
    }

    return (Vec2){ max_width, lines * line_height };
}

void bitmap_font_unload(Bitmap_Font *bitmapFont)
{
    if (!bitmapFont)
        return;

    texture2d_unload(&bitmapFont->texture);
}

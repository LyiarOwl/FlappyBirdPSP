#include "texture2d.h"
#include "common.h"

#include "psputils.h"
#include "stb_image.h"
#include <stdbool.h>

Texture2D texture2d_load(const char *filepath)
{
    if (!filepath) {
        l_assert(false, "[>] texture_load: you forgot to pass the path bitch");
        return (Texture2D){ 0 };
    }

    Texture2D texture = { 0 };

    texture.data =
        (u8 *)stbi_load(filepath, &texture.w, &texture.h, NULL, STBI_rgb_alpha);
    if (!texture.data) {
        l_assert(false, stbi_failure_reason());
        return texture;
    }
    texture.available = true;

    sceKernelDcacheWritebackRange(texture.data, texture.w * texture.h * 4);

    return texture;
}

void texture2d_unload(Texture2D *texture)
{
    if (!texture)
        return;

    stbi_image_free(texture->data);
    texture->available = false;
}

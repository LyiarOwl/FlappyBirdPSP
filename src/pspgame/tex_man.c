#include "tex_man.h"
#include <stdlib.h>
#include <string.h>

typedef struct
{
    char     *path;
    Texture2D texture;
} Texture_Asset;

struct Tex_Manager
{
    size_t         count, capacity;
    Texture_Asset *items;
};

static char *copy_string(const char *src)
{
    size_t n   = strlen(src) + 1;
    char  *out = malloc(n);
    if (out)
        memcpy(out, src, n);
    return out;
}

Tex_Manager *tex_man_new(size_t capacity)
{
    if (!capacity)
        return NULL;
    Tex_Manager *m = calloc(1, sizeof(*m));
    if (!m)
        return NULL;
    m->items = calloc(capacity, sizeof(*m->items));
    if (!m->items) {
        free(m);
        return NULL;
    }
    m->capacity = capacity;
    return m;
}

Texture2D tex_man_load_tex(Tex_Manager *m, const char *path)
{
    Texture2D empty = { 0 };
    if (!m || !path)
        return empty;

    for (size_t i = 0; i < m->count; ++i)
        if (strcmp(path, m->items[i].path) == 0)
            return m->items[i].texture;

    if (m->count == m->capacity)
        return empty;

    char *saved_path = copy_string(path);
    if (!saved_path)
        return empty;

    Texture2D texture = texture2d_load(path);
    if (!texture.available) {
        free(saved_path);
        return empty;
    }

    m->items[m->count++] = (Texture_Asset){ saved_path, texture };
    return texture;
}

void tex_man_unload_tex(Tex_Manager *m, const char *path)
{
    if (!m || !path)
        return;
    for (size_t i = 0; i < m->count; ++i) {
        if (strcmp(path, m->items[i].path) != 0)
            continue;
        texture2d_unload(&m->items[i].texture);
        // UnloadTexture(m->items[i].texture);
        free(m->items[i].path);
        m->items[i] = m->items[--m->count];
        return;
    }
}

void tex_man_free(Tex_Manager *m)
{
    if (!m)
        return;
    for (size_t i = 0; i < m->count; ++i) {
        // UnloadTexture(m->items[i].texture);
        texture2d_unload(&m->items[i].texture);
        free(m->items[i].path);
    }
    free(m->items);
    free(m);
}

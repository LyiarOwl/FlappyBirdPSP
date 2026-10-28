#ifndef TEX_MAN_H
#define TEX_MAN_H
#include "lframework/texture2d.h"
#include <stddef.h>

/** texture manager */
typedef struct Tex_Manager Tex_Manager;

Tex_Manager *tex_man_new(size_t capacity);
/** load or retrieve an already loaded texture. */
Texture2D    tex_man_load_tex(Tex_Manager *manager, const char *path);
void         tex_man_unload_tex(Tex_Manager *manager, const char *path);
void         tex_man_free(Tex_Manager *manager);

#endif

#ifndef SCREEN_MANAGER_H
#define SCREEN_MANAGER_H
#include <stdbool.h>
#include <stddef.h>

typedef struct Screen_Manager Screen_Manager;

typedef enum { LOAD_MORE, LOAD_DONE, LOAD_ERROR } Load_Result;

typedef struct
{
    void *ctx;
    void (*enter)(void *ctx, Screen_Manager *m);
    void (*update)(void *ctx, Screen_Manager *m);
    void (*draw)(void *ctx, const Screen_Manager *m);
    void (*exit)(void *ctx, Screen_Manager *m);
    Load_Result (*load)(void *ctx, Screen_Manager *m);
} Screen_Desc;

Screen_Manager *
screen_manager_new(const Screen_Desc *defs, size_t count, size_t initial, size_t loading);
void screen_manager_request(Screen_Manager *, size_t id);
void screen_manager_update(Screen_Manager *);
void screen_manager_draw(const Screen_Manager *);
void screen_manager_commit(Screen_Manager *);
bool screen_manager_failed(const Screen_Manager *);
void screen_manager_retry(Screen_Manager *);
void screen_manager_free(Screen_Manager *);
#endif

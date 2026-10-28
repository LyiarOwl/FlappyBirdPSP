#include "screen_manager.h"
#include <stdlib.h>

struct Screen_Manager
{
    const Screen_Desc *defs;
    size_t             count, current, loading, target, requested;
    bool               pending, ready, failed;
};

static void enter(Screen_Manager *m, size_t i)
{
    const Screen_Desc *d = &m->defs[i];
    if (d->enter)
        d->enter(d->ctx, m);
}

static void leave(Screen_Manager *m, size_t i)
{
    const Screen_Desc *d = &m->defs[i];
    if (d->exit)
        d->exit(d->ctx, m);
}

Screen_Manager *screen_manager_new(const Screen_Desc *d, size_t n, size_t initial, size_t loading)
{
    if (!d || !n || initial >= n || loading >= n || initial == loading)
        return NULL;
    Screen_Manager *m = calloc(1, sizeof(*m));
    if (!m)
        return NULL;
    m->defs    = d;
    m->count   = n;
    m->current = m->target = initial;
    m->loading             = loading;
    enter(m, initial);
    return m;
}

void screen_manager_request(Screen_Manager *m, size_t id)
{
    if (!m || id >= m->count || id == m->loading || m->current == m->loading)
        return;
    m->requested = id;
    m->pending   = true;
}

void screen_manager_update(Screen_Manager *m)
{
    if (!m)
        return;
    const Screen_Desc *d = &m->defs[m->current];
    if (d->update)
        d->update(d->ctx, m);
    if (m->current == m->loading && !m->failed && !m->ready) {
        d             = &m->defs[m->target];
        Load_Result r = d->load(d->ctx, m);
        if (r == LOAD_DONE)
            m->ready = true;
        else if (r == LOAD_ERROR)
            m->failed = true;
    }
}

void screen_manager_draw(const Screen_Manager *m)
{
    if (!m)
        return;
    const Screen_Desc *d = &m->defs[m->current];
    if (d->draw)
        d->draw(d->ctx, m);
}

void screen_manager_commit(Screen_Manager *m)
{
    if (!m)
        return;
    if (m->current == m->loading) {
        if (!m->ready)
            return;
        leave(m, m->loading);
        m->current = m->target;
        m->ready   = false;
        enter(m, m->current);
        return;
    }
    if (!m->pending)
        return;
    size_t id  = m->requested;
    m->pending = false;
    if (id == m->current)
        return;
    leave(m, m->current);
    m->target = id;
    if (m->defs[id].load) {
        m->current = m->loading;
        m->failed = m->ready = false;
    } else
        m->current = id;
    enter(m, m->current);
}

bool screen_manager_failed(const Screen_Manager *m) { return m && m->failed; }

void screen_manager_retry(Screen_Manager *m)
{
    if (m && m->current == m->loading)
        m->failed = false;
}

void screen_manager_free(Screen_Manager *m)
{
    if (!m)
        return;
    leave(m, m->current);
    free(m);
}

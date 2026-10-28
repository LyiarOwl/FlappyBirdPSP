#include "phw_character_body.h"
#include "phw_vec2.h"
#include "phw_world.h"

Phw_Vec2 phw_body_get_position(const Phw_Character_Body *body)
{
    return body ? body->bbox.position : (Phw_Vec2){ 0.f, 0.f };
}

bool phw_body_try_set_position(Phw_Character_Body *body, Phw_Vec2 pos)
{
    if (!body || !body->active || body->is_static || !body->world)
        return false;
    if (phw_vec2_equals(body->bbox.position, pos))
        return true;

    Phw_Vec2 old = body->bbox.position;
    body->bbox.position = pos;
    if (phw_world_update_body(body->world, body))
        return true;
    body->bbox.position = old;
    return false;
}

void phw_body_set_position(Phw_Character_Body *body, Phw_Vec2 pos)
{
    (void)phw_body_try_set_position(body, pos);
}

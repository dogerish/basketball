#include "entity.h"
#include "physics.h"
#include <SDL2/SDL.h>

void entityUpdate(entity* ent, double dt)
{
    ent->vel.x += ent->acc.x * dt;
    ent->vel.y += ent->acc.y * dt;
    ent->vel.z += ent->acc.z * dt;
    ent->acc.x = 0;
    ent->acc.y = 0;
    ent->acc.z = 0;
    ent->pos.x += ent->vel.x * dt;
    ent->pos.y += ent->vel.y * dt;
    ent->pos.z += ent->vel.z * dt;
}

void entityApplyForce(entity* ent, float x, float y, float z)
{
    ent->acc.x += x;
    ent->acc.y += y;
    ent->acc.z += z;
}

void entityUpdateRect(entity* ent, SDL_Rect* r)
{
    r->x = world_to_screen(ent->pos.x, ent->pos.z);
    r->y = world_to_screen(ent->pos.y, ent->pos.z);
    r->w = world_to_screen(ent->dim.x, ent->pos.z);
    r->h = world_to_screen(ent->dim.y, ent->pos.z);
}

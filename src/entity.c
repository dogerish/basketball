#include "entity.h"
#include "SDL2/SDL.h"

void entityUpdate(entity* ent)
{
    ent->vel.x += ent->acc.x;
    ent->vel.y += ent->acc.y;
    ent->acc.x = 0;
    ent->acc.y = 0;
    ent->pos.x += ent->vel.x;
    ent->pos.y += ent->vel.y;
}

void entityApplyForce(entity* ent, float x, float y)
{
    ent->acc.x += x;
    ent->acc.y += y;
}

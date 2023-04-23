#pragma once
#include "physics.h"
#include <SDL2/SDL.h>

typedef struct
{
    vec3d pos;
    vec3d acc;
    vec3d vel;
    vec3d dim; // dimensions
    float coef_rest; // coefficient of restitution; how bouncy: 0 (all energy 
                     // lost) to 1 (no energy lost)
} entity;

void entityUpdate(entity* ent, double dt);

void entityApplyForce(entity* ent, float x, float y, float z);

void entityUpdateRect(entity* ent, SDL_Rect* r);

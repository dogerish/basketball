#pragma once
#include "SDL2/SDL.h"

typedef struct
{
    SDL_FPoint pos;
    SDL_FPoint acc;
    SDL_FPoint vel;
} entity;

void entityUpdate(entity* ent);

void entityApplyForce(entity* ent, float x, float y);

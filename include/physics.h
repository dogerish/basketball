#pragma once
#include <SDL2/SDL.h>

#define GRAVITY 9.8
// screen unit (pixels) per meter (world unit) at depth z = 1 world unit
#define SCREEN_PER_WORLD 100

typedef struct {
    float x, y, z;
} vec3d;

int world_to_screen(float p, float z);

float screen_to_world(int p, float z);

#include "physics.h"

int world_to_screen(float p, float z)
{
    return p / z * SCREEN_PER_WORLD;
}

float screen_to_world(int p, float z)
{
    return p * z / SCREEN_PER_WORLD;
}

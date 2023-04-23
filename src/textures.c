#include "textures.h"
#include <SDL2/SDL.h>

SDL_Texture* LoadBMPTexture(SDL_Renderer* renderer, const char* fname)
{
    SDL_Surface* s = SDL_LoadBMP(fname);
    if (!s)
        return NULL;
    SDL_Texture* t = SDL_CreateTextureFromSurface(renderer, s);
    SDL_FreeSurface(s);
    return t;
}

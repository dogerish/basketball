#include "stdio.h"
#include "SDL2/SDL.h"
#include "textures.h"
#include "entity.h"

enum ExitCode
{
    EX_SUCCESS = 0,
    EX_EINIT = 1,
    EX_EWINREN = 2,
    EX_ENOTEX = 3,
};

#define OHNO(context) SDL_LogError(SDL_LOG_CATEGORY_ASSERT, context " (%s:%i, %s)\n", __FILE__, __LINE__, SDL_GetError());

int main(int argc, char* argv[])
{
    SDL_Window* win = NULL;
    SDL_Renderer* ren = NULL;
    int w = 420, h = 420;
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS))
    {
        OHNO("SDL Initialization failed");
        return EX_EINIT;
    }
    // set verbose as the minimum prirority required for printing
    /* SDL_LogSetPriority(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_VERBOSE); */

    if (SDL_CreateWindowAndRenderer(w, h, 0, &win, &ren))
    {
        OHNO("Failed to create window and renderer");
        return EX_EWINREN;
    }
    SDL_LogVerbose(SDL_LOG_CATEGORY_APPLICATION, "Created window and renderer\n");

    // load texture
    SDL_Texture* person = LoadBMPTexture(ren, "textures/person.bmp");
    SDL_Texture* basketball = LoadBMPTexture(ren, "textures/basketball.bmp");
    SDL_Texture* hoop = LoadBMPTexture(ren, "textures/hoop.bmp");
    if (!person || !basketball || !hoop)
    {
        OHNO("Failed to load texture");
        SDL_DestroyRenderer(ren);
        SDL_DestroyWindow(win);
        SDL_Quit();
        return EX_ENOTEX;
    }
    SDL_SetTextureBlendMode(person, SDL_BLENDMODE_BLEND);
    SDL_SetTextureBlendMode(basketball, SDL_BLENDMODE_BLEND);
    SDL_SetTextureBlendMode(hoop, SDL_BLENDMODE_BLEND);
    SDL_Rect personrect = { w - 50, h - 100, 50, 100 };
    SDL_Rect bballrect = { personrect.x - 5, personrect.y + 10, 20, 20 };
    SDL_Rect hooprect = { 0, h/2, 100, 75 };
    SDL_Rect netrect = {
        hooprect.x + hooprect.w / 2, hooprect.y + hooprect.h / 2,
        hooprect.w / 3,              hooprect.h / 4
    };
    netrect.x -= netrect.w / 2;
    SDL_Rect screenrect = { 0, 0, w, h };

    SDL_FPoint bballspawn = { bballrect.x, bballrect.y };
    entity bballent = { bballspawn, 0, 0, 0, 0 };

    // main loop
    int running = 1;
    int ballIsHeld = 1;
    int ballInNet = 0;
    int score = 0;
    while (running)
    {
        // gravity
        entityApplyForce(&bballent, 0, 0.5);

        // respawn ball if out of screen (except if above screen)
        if (!SDL_HasIntersection(&bballrect, &screenrect) &&
            bballrect.y + bballrect.h >= screenrect.y)
        {
            bballent.pos = bballspawn;
            ballIsHeld = 1;
            bballent.vel.x = 0;
            bballent.vel.y = 0;
        }

        if (SDL_HasIntersection(&bballrect, &netrect))
        {
            if (!ballInNet && bballent.vel.y > 0)
            {
                // x intersection with the top edge of the net rectangle, 
                // assuming it crossed from over net to inside net
                float bx = bballent.pos.x + bballrect.w / 2,
                      by = bballent.pos.y + bballrect.h / 2;
                float xint = bx + bballent.vel.x / bballent.vel.y * (netrect.y - by);
                if (xint > netrect.x && xint < netrect.x + netrect.w)
                {
                    ballInNet = 1;
                    score++;
                    printf("Score: %i\n", score);
                }
            }
        }
        else if (ballInNet)
        {
            ballInNet = 0;
        }

        // event loop
        for (SDL_Event e; SDL_PollEvent(&e); )
        {
            switch (e.type)
            {
            case SDL_QUIT:
                running = 0;
                break;
            case SDL_MOUSEBUTTONDOWN:
                if (!ballIsHeld) break;
                ballIsHeld = 0;
                float x = (float) (e.button.x - bballent.pos.x) / 10;
                float y = (float) (e.button.y - bballent.pos.y) / 10;
                entityApplyForce(&bballent, x, y);
                break;
            }
        }
        if (ballIsHeld) entityApplyForce(&bballent, 0, -0.5);

        // update ball
        entityUpdate(&bballent);
        bballrect.x = bballent.pos.x;
        bballrect.y = bballent.pos.y;

        // render background
        SDL_SetRenderDrawColor(ren, ballInNet*100, 100, 255, 255);
        SDL_RenderClear(ren);

        // render textures
        SDL_RenderCopy(ren, person, NULL, &personrect);
        if (ballIsHeld)
        {
            SDL_SetRenderDrawColor(ren, 255, 255, 255, 255);
            int x, y;
            SDL_GetMouseState(&x, &y);
            SDL_RenderDrawLine(ren, bballrect.x, bballrect.y, x, y);
        }
        SDL_RenderCopy(ren, hoop, NULL, &hooprect);
        /* SDL_SetRenderDrawColor(ren, 0, 0, 0, 255); */
        /* SDL_RenderDrawRect(ren, &netrect); */
        SDL_RenderCopy(ren, basketball, NULL, &bballrect);

        SDL_RenderPresent(ren);
        SDL_Delay(15);
    }

    // exit
    SDL_DestroyTexture(person);
    SDL_DestroyTexture(basketball);
    SDL_DestroyTexture(hoop);
    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    SDL_Quit();
    return EX_SUCCESS;
}

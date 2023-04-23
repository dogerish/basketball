#include <stdio.h>
#include <time.h>
#include <SDL2/SDL.h>
#include "textures.h"
#include "entity.h"
#include "physics.h"

enum ExitCode
{
    EX_SUCCESS = 0,
    EX_EINIT = 1,
    EX_EWINREN = 2,
    EX_ENOTEX = 3,
};

#define OHNO(context) SDL_LogError(SDL_LOG_CATEGORY_ASSERT, context " (%s:%i, %s)\n", __FILE__, __LINE__, SDL_GetError());
#define TIMESPEC_TO_DOUBLE(ts) ((double) ts.tv_sec + (double) ts.tv_nsec / 1e9)

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

    vec3d bballspawn = { bballrect.x, bballrect.y, 1 };
    bballspawn.x = screen_to_world(bballspawn.x, bballspawn.z);
    bballspawn.y = screen_to_world(bballspawn.y, bballspawn.z);
#define MAXBBALLS 25
    entity bballs[MAXBBALLS];
    entity spawnball = {
        .pos = bballspawn,
        .vel = { 0, 0, 0 },
        .acc = { 0, 0, 0 },
        .dim = { 0.25, 0.25, 0.25 },
        .coef_rest = 0.75
    };
    entity* bballent = bballs;

    // main loop
    int running = 1;
    char flyingbballs[MAXBBALLS] = { 0 };
    char netbballs[MAXBBALLS] = { 0 };
    int score = 0;
    struct timespec lasttick;
    clock_gettime(CLOCK_MONOTONIC, &lasttick);
    struct timespec ticks;
    clock_gettime(CLOCK_MONOTONIC, &ticks);
    double dt = 0;
    while (running)
    {
        for (int i = 0; i < MAXBBALLS; i++)
        {
            if (!flyingbballs[i]) continue;
            bballent = bballs + i;

            // gravity
            entityApplyForce(bballent, 0, GRAVITY, 0);

            // respawn ball at horizontal screen edges
            entityUpdateRect(bballent, &bballrect);
            if (bballrect.x + bballrect.w <= screenrect.x ||
                bballrect.x >= screenrect.x + screenrect.w)
            {
                *bballent = spawnball;
                flyingbballs[i] = 0;
            }
            // bounce ball at bottom of screen
            else if (bballrect.y + bballrect.h >= screenrect.y + screenrect.h)
            {
                bballent->pos.y = screen_to_world(screenrect.y + screenrect.h - bballrect.h, bballent->pos.z);
                // apply bounce
                if (bballent->vel.y > 0.1f) bballent->vel.y *= -bballent->coef_rest;
                // cancel gravity
                else entityApplyForce(bballent, 0, -GRAVITY, 0);
            }

            if (SDL_HasIntersection(&bballrect, &netrect))
            {
                if (!netbballs[i] && bballent->vel.y > 0)
                {
                    // x intersection with the top edge of the net rectangle, 
                    // assuming it crossed from over net to inside net
                    float bx = bballrect.x + bballrect.w / 2,
                          by = bballrect.y + bballrect.h / 2;
                    float xint = bx + world_to_screen(bballent->vel.x, bballent->pos.z) / world_to_screen(bballent->vel.y, bballent->pos.z) * (netrect.y - by);
                    if (xint > netrect.x && xint < netrect.x + netrect.w)
                    {
                        netbballs[i] = 1;
                        score++;
                        printf("Score: %i\n", score);
                    }
                }
            }
            else if (netbballs[i])
            {
                netbballs[i] = 0;
            }
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
                // throw next non-flying basketball
                for (int i = 0; i < MAXBBALLS; i++)
                {
                    if (flyingbballs[i]) continue;

                    flyingbballs[i] = 1;
                    entityUpdateRect(bballent = bballs + i, &bballrect);
                    float x = screen_to_world((e.button.x - bballrect.x)/dt*5, bballent->pos.z);
                    float y = screen_to_world((e.button.y - bballrect.y)/dt*5, bballent->pos.z);
                    entityApplyForce(bballent, x, y, 0);
                    break;
                }
                break;
            }
        }
        for (int i = 0; i < MAXBBALLS; i++)
        {
            if (!flyingbballs[i]) continue;
            bballent = bballs + i;
            // update ball
            entityUpdate(bballent, dt);
            entityUpdateRect(bballent, &bballrect);
        }

        // render background
        SDL_SetRenderDrawColor(ren, 0, 100, 255, 255);
        SDL_RenderClear(ren);

        // draw person
        SDL_RenderCopy(ren, person, NULL, &personrect);

        // draw guidance line
        SDL_SetRenderDrawColor(ren, 255, 255, 255, 255);
        int x, y;
        SDL_GetMouseState(&x, &y);
        SDL_RenderDrawLine(ren, world_to_screen(spawnball.pos.x, spawnball.pos.z), world_to_screen(spawnball.pos.y, spawnball.pos.z), x, y);

        // draw hoop and basketballs
        SDL_RenderCopy(ren, hoop, NULL, &hooprect);
        for (int i = 0, h = 0; i < MAXBBALLS; i++)
        {
            // only draw up to one non-flying ball
            if (!flyingbballs[i] && h) continue;
            else if (!flyingbballs[i])
            {
                h = 1;
                bballent = &spawnball;
            }
            else bballent = bballs + i;

            entityUpdateRect(bballent, &bballrect);
            SDL_RenderCopy(ren, basketball, NULL, &bballrect);
        }

        SDL_RenderPresent(ren);
        SDL_Delay(15);
        clock_gettime(CLOCK_MONOTONIC, &ticks);
        dt = TIMESPEC_TO_DOUBLE(ticks) - TIMESPEC_TO_DOUBLE(lasttick);
        clock_gettime(CLOCK_MONOTONIC, &lasttick);
        printf("%04.01f fps\r", 1/dt);
        fflush(stdout);
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

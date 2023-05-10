#include "levels.h"
#include "textures.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>

#define SCREEN_WIDTH 510
#define SCREEN_HEIGHT 510
#define STEP 51

enum direction { LEFT,
                 RIGHT };

enum field {
    EMPTY,
    WALL,
    BOX,
    DST
};

int init();
int loadMedia();
int is_allowed(int point);

SDL_Window *gWindow = NULL;
SDL_Renderer *gRenderer = NULL;

struct gHeroTextures {
    int direction;
    struct LTexture left;
    struct LTexture right;
} gHeroTexture;

struct LTexture gLevelTexture;
struct LTexture gDstTexture;
struct LTexture gBoxTexture;
struct LTexture gWallTexture;

int main(int argc, char *args[])
{
    if (!init()) {
        printf("Failed to initialize!\n");
        return 1;
    }

    if (!loadMedia()) {
        printf("Failed to load media!\n");
        return 1;
    }

    struct level cur_level = get_level(1);

    int Running = 1;
    gHeroTexture.direction = RIGHT;
    SDL_Event e;

    while (Running) {
        SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
        SDL_RenderClear(gRenderer);

        //        renderTexture(&gLevelTexture, 0, 0, gRenderer);

        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                Running = 0;
                break;
            }

            if (e.type == SDL_KEYDOWN)
                switch (e.key.keysym.sym) {
                    case SDLK_r:
                        Running = 0;
                        break;
                    case SDLK_UP:
                        if (cur_level.hero.y != 0) {
                            if (is_allowed(cur_level.field[cur_level.hero.y - 1][cur_level.hero.x]))
                                --cur_level.hero.y;
                            else if (cur_level.hero.y >= 2 &&
                                     cur_level.field[cur_level.hero.y - 1][cur_level.hero.x] == BOX &&
                                     is_allowed(cur_level.field[cur_level.hero.y - 2][cur_level.hero.x])) {
                                cur_level.field[--cur_level.hero.y][cur_level.hero.x] = EMPTY;
                                cur_level.field[cur_level.hero.y - 1][cur_level.hero.x] = BOX;
                            }
                        }
                        break;

                    case SDLK_DOWN:
                        if (cur_level.hero.y < N_FIELDS_HEIGHT - 1) {
                            if (is_allowed(cur_level.field[cur_level.hero.y + 1][cur_level.hero.x]))
                                ++cur_level.hero.y;
                            else if (cur_level.hero.y < N_FIELDS_HEIGHT - 2 &&
                                     cur_level.field[cur_level.hero.y + 1][cur_level.hero.x] == BOX &&
                                     is_allowed(cur_level.field[cur_level.hero.y + 2][cur_level.hero.x])) {
                                cur_level.field[++cur_level.hero.y][cur_level.hero.x] = EMPTY;
                                cur_level.field[cur_level.hero.y + 1][cur_level.hero.x] = BOX;
                            }
                        }
                        break;

                    case SDLK_LEFT:
                        gHeroTexture.direction = LEFT;
                        if (cur_level.hero.x > 0) {
                            if (is_allowed(cur_level.field[cur_level.hero.y][cur_level.hero.x - 1]))
                                --cur_level.hero.x;
                            else if (cur_level.hero.x >= 2 &&
                                     cur_level.field[cur_level.hero.y][cur_level.hero.x - 1] == BOX &&
                                     is_allowed(cur_level.field[cur_level.hero.y][cur_level.hero.x - 2])) {
                                cur_level.field[cur_level.hero.y][--cur_level.hero.x] = EMPTY;
                                cur_level.field[cur_level.hero.y][cur_level.hero.x - 1] = BOX;
                            }
                        }
                        break;

                    case SDLK_RIGHT:
                        gHeroTexture.direction = RIGHT;
                        if (cur_level.hero.x < N_FIELDS_WIDTH - 1) {
                            if (is_allowed(cur_level.field[cur_level.hero.y][cur_level.hero.x + 1]))
                                ++cur_level.hero.x;
                            else if (cur_level.hero.x < N_FIELDS_WIDTH - 2 &&
                                     cur_level.field[cur_level.hero.y][cur_level.hero.x + 1] == BOX &&
                                     is_allowed(cur_level.field[cur_level.hero.y][cur_level.hero.x + 2])) {
                                cur_level.field[cur_level.hero.y][++cur_level.hero.x] = EMPTY;
                                cur_level.field[cur_level.hero.y][cur_level.hero.x + 1] = BOX;
                            }
                        }
                        break;
                }
        }

        for (int y = 0; y < N_FIELDS_HEIGHT; ++y)
            for (int x = 0; x < N_FIELDS_WIDTH; ++x)
            {
                switch (cur_level.field[y][x]) {
                    case BOX:
                        renderTexture(&gBoxTexture, x * STEP, y * STEP, gRenderer);
                        break;
                    case WALL:
                        renderTexture(&gWallTexture, x * STEP, y * STEP, gRenderer);
                        break;
                    case DST:
                        renderTexture(&gDstTexture, x * STEP, y * STEP, gRenderer);
                        break;
                    default:
                        break;
                }
            }

        if (gHeroTexture.direction == RIGHT)
            renderTexture(&gHeroTexture.right, cur_level.hero.x * STEP, cur_level.hero.y * STEP, gRenderer);
        else
            renderTexture(&gHeroTexture.left, cur_level.hero.x * STEP, cur_level.hero.y * STEP, gRenderer);

        SDL_RenderPresent(gRenderer);
    }

    return 0;
}

int is_allowed(int point)
{
    if (point == EMPTY || point == DST)
        return 1;

    return 0;
}

int init()
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL init Error: %s\n", SDL_GetError());
        return 0;
    }

    if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1")) {
        printf("Warning: Linear texture filtering not enabled!");
    }

    gWindow = SDL_CreateWindow("Boxes Game",
                               SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                               SCREEN_WIDTH, SCREEN_HEIGHT,
                               SDL_WINDOW_SHOWN);
    if (gWindow == NULL) {
        printf("SDL window creating Error: %s\n", SDL_GetError());
        return 0;
    }

    gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED);
    if (gRenderer == NULL) {
        printf("SDL renderer creating Error: %s\n", SDL_GetError());
        return 0;
    }

    SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);

    int imgFlags = IMG_INIT_PNG;
    if (!(IMG_Init(imgFlags) & imgFlags)) {
        printf("SDL_image init Error: %s\n", IMG_GetError());
        return 0;
    }

    return 1;
}

int loadMedia()
{
    if (!loadTextureFromFile(&gHeroTexture.right, "./images/temp/rb_right.bmp", gRenderer, STEP, STEP)) {
        printf("Texture image loading Error.\n");
        return 0;
    }

    if (!loadTextureFromFile(&gHeroTexture.left, "./images/temp/rb_left.bmp", gRenderer, STEP, STEP)) {
        printf("Texture image loading Error.\n");
        return 0;
    }

    //    if (!loadTextureFromFile(&gLevelTexture, "./images/temp/grass.bmp", gRenderer, SCREEN_WIDTH, SCREEN_HEIGHT)) {
    //        printf("Texture image loading Error.\n");
    //        return 0;
    //    }

    if (!loadTextureFromFile(&gBoxTexture, "./images/temp/box_80.bmp", gRenderer, STEP, STEP)) {
        printf("Texture image loading Error.\n");
        return 0;
    }

    if (!loadTextureFromFile(&gDstTexture, "./images/temp/x.bmp", gRenderer, STEP, STEP)) {
        printf("Texture image loading Error.\n");
        return 0;
    }

    if (!loadTextureFromFile(&gWallTexture, "./images/classic/box.png", gRenderer, STEP, STEP)) {
        printf("Texture image loading Error.\n");
        return 0;
    }

    return 1;
}

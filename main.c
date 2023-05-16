#include "levels.h"
#include "textures.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <sys/time.h>
#include <time.h>

#define SCREEN_WIDTH 510
#define SCREEN_HEIGHT 510
#define STEP 51

#define MSEC_IN_SEC 1000
#define USEC_IN_MSEC 1000
#define FPS 30

enum direction { LEFT,
                 RIGHT };

enum field {
    EMPTY,
    WALL,
    BOX
};

int init();
int loadMedia();
long long time_calc(const struct timeval *start, const struct timeval *end);

SDL_Window *gWindow = NULL;
SDL_Renderer *gRenderer = NULL;

struct gHeroTextures {
    int direction;
    struct LTexture left;
    struct LTexture right;
} gHeroTexture;

struct LTexture gDstTexture;
struct LTexture gBoxTexture;
struct LTexture gWallTexture;

int main(int argc, char *args[])
{
    /* TODO(Talkasi): add logs */
    if (!init()) {
        printf("Failed to initialize!\n");
        return 1;
    }

    if (!loadMedia()) {
        printf("Failed to load media!\n");
        return 1;
    }

    int GameRunning = 1;
    int level_n = 1;
    SDL_Event e;

    while (GameRunning) {
        if (level_n > N_LEVELS)
            break;

        struct levelField cur_level;
        get_level(level_n, &cur_level);
        int LevelRunning = 1;

        gHeroTexture.direction = RIGHT;

        while (LevelRunning) {
            struct timeval start, end;
            gettimeofday(&start, 0);

            SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
            SDL_RenderClear(gRenderer);

            while (SDL_PollEvent(&e) != 0) {
                if (e.type == SDL_QUIT) {
                    GameRunning = 0;
                    LevelRunning = 0;
                    break;
                }

                if (e.type == SDL_KEYDOWN)
                    switch (e.key.keysym.sym) {
                        case SDLK_r:
                            LevelRunning = 0;
                            break;
                        case SDLK_UP:
                            if (cur_level.hero.i > 1 && cur_level.field[cur_level.hero.i - 1][cur_level.hero.j].type == EMPTY) {
                                --cur_level.hero.i;
                                cur_level.hero.y -= STEP;
                            } else if (cur_level.hero.i >= 2 &&
                                       cur_level.field[cur_level.hero.i - 1][cur_level.hero.j].type == BOX &&
                                       cur_level.field[cur_level.hero.i - 2][cur_level.hero.j].type == EMPTY) {
                                cur_level.field[--cur_level.hero.i][cur_level.hero.j].type = EMPTY;
                                cur_level.field[cur_level.hero.i - 1][cur_level.hero.j].type = BOX;
                                cur_level.hero.y -= STEP;
                            }
                            break;

                        case SDLK_DOWN:
                            if (cur_level.hero.i < N_FIELDS_HEIGHT - 1) {
                                if (cur_level.field[cur_level.hero.i + 1][cur_level.hero.j].type == EMPTY) {
                                    ++cur_level.hero.i;
                                    cur_level.hero.y += STEP;
                                } else if (cur_level.hero.i < N_FIELDS_HEIGHT - 2 &&
                                           cur_level.field[cur_level.hero.i + 1][cur_level.hero.j].type == BOX &&
                                           cur_level.field[cur_level.hero.i + 2][cur_level.hero.j].type == EMPTY) {
                                    cur_level.field[++cur_level.hero.i][cur_level.hero.j].type = EMPTY;
                                    cur_level.field[cur_level.hero.i + 1][cur_level.hero.j].type = BOX;
                                    cur_level.hero.y += STEP;
                                }
                            }
                            break;

                        case SDLK_LEFT:
                            gHeroTexture.direction = LEFT;
                            if (cur_level.hero.j > 0) {
                                if (cur_level.field[cur_level.hero.i][cur_level.hero.j - 1].type == EMPTY) {
                                    --cur_level.hero.j;
                                    cur_level.hero.x -= STEP;
                                } else if (cur_level.hero.j >= 2 &&
                                         cur_level.field[cur_level.hero.i][cur_level.hero.j - 1].type == BOX &&
                                         cur_level.field[cur_level.hero.i][cur_level.hero.j - 2].type == EMPTY) {
                                    cur_level.field[cur_level.hero.i][--cur_level.hero.j].type = EMPTY;
                                    cur_level.field[cur_level.hero.i][cur_level.hero.j - 1].type = BOX;
                                    cur_level.hero.x -= STEP;
                                }
                            }
                            break;

                        case SDLK_RIGHT:
                            gHeroTexture.direction = RIGHT;
                            if (cur_level.hero.j < N_FIELDS_WIDTH - 1) {
                                if (cur_level.field[cur_level.hero.i][cur_level.hero.j + 1].type == EMPTY) {
                                    ++cur_level.hero.j;
                                    cur_level.hero.x += STEP;
                                } else if (cur_level.hero.j < N_FIELDS_WIDTH - 2 &&
                                         cur_level.field[cur_level.hero.i][cur_level.hero.j + 1].type == BOX &&
                                         cur_level.field[cur_level.hero.i][cur_level.hero.j + 2].type == EMPTY) {
                                    cur_level.field[cur_level.hero.i][++cur_level.hero.j].type = EMPTY;
                                    cur_level.field[cur_level.hero.i][cur_level.hero.j + 1].type = BOX;
                                    cur_level.hero.x += STEP;
                                }
                            }
                            break;
                    }
            }

            /* NOTE: gDstTexture now is rendered even if it is not needed. */
            /* TODO(Talkasi): rewrite it to be more optimized */
            for (int n = 0; n < cur_level.n_boxes; ++n) {
                renderTexture(&gDstTexture, cur_level.dst[n].x, cur_level.dst[n].y, gRenderer);
            }

            int progress = 0;
            for (int y = 0; y < N_FIELDS_HEIGHT; ++y)
                for (int x = 0; x < N_FIELDS_WIDTH; ++x) {
                    switch (cur_level.field[y][x].type) {
                        case BOX:
                            for (int dst = 0; dst < cur_level.n_boxes; ++dst)
                                if (cur_level.field[y][x].x == cur_level.dst[dst].x && cur_level.field[y][x].y == cur_level.dst[dst].y) {
                                    ++progress;
                                    /* TODO(Talkasi): render box on dst */
                                    continue;
                                }

                            renderTexture(&gBoxTexture, cur_level.field[y][x].x, cur_level.field[y][x].y, gRenderer);
                            break;
                        case WALL:
                            renderTexture(&gWallTexture, cur_level.field[y][x].x, cur_level.field[y][x].y, gRenderer);
                            break;
                        default:
                            break;
                    }
                }

            if (progress == cur_level.n_boxes) {
                LevelRunning = 0;
                ++level_n;
                /* TODO(Talkasi): add menu with music */
            }

            if (gHeroTexture.direction == RIGHT)
                renderTexture(&gHeroTexture.right, cur_level.hero.x, cur_level.hero.y, gRenderer);
            else
                renderTexture(&gHeroTexture.left, cur_level.hero.x, cur_level.hero.y, gRenderer);

            SDL_RenderPresent(gRenderer);

            gettimeofday(&end, 0);
            long long t = MSEC_IN_SEC / FPS - time_calc(&start, &end);
            if (t > 0) {
                struct timespec req = {t / MSEC_IN_SEC, t % MSEC_IN_SEC};
                nanosleep(&req, 0);
            }
        }
    }

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

long long time_calc(const struct timeval *start, const struct timeval *end)
{
    return (long long) (end->tv_sec - start->tv_sec) * MSEC_IN_SEC +
           (end->tv_usec - start->tv_usec) / USEC_IN_MSEC;
}

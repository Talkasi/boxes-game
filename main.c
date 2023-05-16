#include "levels.h"
#include "textures.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <stdio.h>

#define SCREEN_WIDTH 510
#define SCREEN_HEIGHT 510
#define STEP 51

#define MSEC_IN_SEC 1000
#define FPS 30

int init();
int loadMedia();

enum hero_state { LEFT,
                  RIGHT,
                  SUCCESS };

enum field {
    EMPTY,
    WALL,
    BOX
};

enum music_type {
    NO_MUSIC,
    LEVEL,
    SUCCESS_START,
    SUCCESS_END
};

SDL_Window *gWindow = NULL;
SDL_Renderer *gRenderer = NULL;

struct gHeroTextures {
    int state;
    struct LTexture left;
    struct LTexture right;
    struct LTexture success;
} gHeroTexture;

struct LTexture gDstTexture;
struct LTexture gBoxTexture;
struct LTexture gWallTexture;

Mix_Music *gMusic = NULL;
Mix_Music *gSuccessStartMusic = NULL;
Mix_Music *gSuccessEndMusic = NULL;

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

        struct level cur_level;
        get_level(level_n, &cur_level);
        int LevelRunning = 1;
        int MusicType = LEVEL;

        gHeroTexture.state = RIGHT;

        Mix_PlayMusic(gMusic, -1);
        while (LevelRunning) {
            uint32_t start, delay_time;
            start = SDL_GetTicks();
            if (Mix_PlayingMusic() == 0)
                switch (MusicType) {
                    case SUCCESS_START:
                        Mix_PlayMusic(gSuccessStartMusic, 1);
                        MusicType = SUCCESS_END;
                        break;
                    case SUCCESS_END:
                        Mix_PlayMusic(gSuccessEndMusic, 1);
                        gHeroTexture.state = SUCCESS;
                        MusicType = NO_MUSIC;
                        break;
                    case NO_MUSIC:
                        LevelRunning = 0;
                        ++level_n;
                        break;
                    default:
                        break;
                }

            SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
            SDL_RenderClear(gRenderer);

            while (SDL_PollEvent(&e) != 0) {
                if (e.type == SDL_QUIT) {
                    GameRunning = 0;
                    LevelRunning = 0;
                    break;
                }

                if (MusicType == LEVEL && e.type == SDL_KEYDOWN)
                    switch (e.key.keysym.sym) {
                        case SDLK_r:
                            LevelRunning = 0;
                            break;
                        case SDLK_UP:
                            if (cur_level.hero.i >= 1 && cur_level.field[cur_level.hero.i - 1][cur_level.hero.j].type == EMPTY) {
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
                            gHeroTexture.state = LEFT;
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
                            gHeroTexture.state = RIGHT;
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

            if (progress == cur_level.n_boxes && MusicType == LEVEL) {
                MusicType = SUCCESS_START;
                Mix_HaltMusic();
                /* TODO(Talkasi): add menu */
            }

            switch (gHeroTexture.state) {
                case RIGHT:
                    renderTexture(&gHeroTexture.right, cur_level.hero.x, cur_level.hero.y, gRenderer);
                    break;
                case LEFT:
                    renderTexture(&gHeroTexture.left, cur_level.hero.x, cur_level.hero.y, gRenderer);
                    break;
                case SUCCESS:
                    renderTexture(&gHeroTexture.success, cur_level.hero.x, cur_level.hero.y, gRenderer);
                    break;
            }

            SDL_RenderPresent(gRenderer);

            delay_time = MSEC_IN_SEC / FPS - start + SDL_GetTicks();
            if (delay_time > 0) {
                SDL_Delay(delay_time);
            }
        }
    }

    return 0;
}

int init()
{
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_VIDEO) < 0) {
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

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        printf("SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError());
        return 0;
    }

    return 1;
}

int loadMedia()
{
    gMusic = Mix_LoadMUS("music/level.mp3");
    if (gMusic == NULL) {
        printf("Failed to load music. SDL_mixer Error: %s\n", Mix_GetError());
        return 0;
    }

    gSuccessStartMusic = Mix_LoadMUS("music/jingle_01.mp3");
    if (gSuccessStartMusic == NULL) {
        printf("Failed to load music. SDL_mixer Error: %s\n", Mix_GetError());
        return 0;
    }

    gSuccessEndMusic = Mix_LoadMUS("music/jingle_02.mp3");
    if (gSuccessEndMusic == NULL) {
        printf("Failed to load music. SDL_mixer Error: %s\n", Mix_GetError());
        return 0;
    }

    if (!loadTextureFromFile(&gHeroTexture.right, "./images/temp/rb_right.bmp", gRenderer, STEP, STEP)) {
        printf("Texture image loading Error.\n");
        return 0;
    }

    if (!loadTextureFromFile(&gHeroTexture.left, "./images/temp/rb_left.bmp", gRenderer, STEP, STEP)) {
        printf("Texture image loading Error.\n");
        return 0;
    }

    if (!loadTextureFromFile(&gHeroTexture.success, "./images/level/bobr.png", gRenderer, STEP, STEP)) {
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
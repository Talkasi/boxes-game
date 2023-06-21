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
int RenderField(struct level *cur_level);
int inBorders(int curPos_i, int curPos_j, int curVel_i, int curVel_j);
int setTextures(SDL_Rect Textures[], int n, int w, int h);

enum hero_state { RIGHT,
                  LEFT,
                  SUCCESS,
                  N_HERO_TYPES };

enum music_type {
    NO_MUSIC,
    LEVEL,
    SUCCESS_START,
    SUCCESS_END
};

SDL_Window *gWindow = NULL;
SDL_Renderer *gRenderer = NULL;

struct LTexture gHeroTextures;
SDL_Rect HeroT[N_HERO_TYPES];

struct LTexture gFieldTextures;
SDL_Rect FieldT[N_FIELD_TYPES];

Mix_Music *gMusic = NULL;
Mix_Music *gSuccessStartMusic = NULL;
Mix_Music *gSuccessEndMusic = NULL;

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
        int HeroState = RIGHT;

        while (LevelRunning) {
            uint32_t start, delay_time;
            start = SDL_GetTicks();

            if (Mix_PlayingMusic() == 0)
                switch (MusicType) {
                    case LEVEL:
                        Mix_PlayMusic(gMusic, -1);
                        break;
                    case SUCCESS_START:
                        Mix_PlayMusic(gSuccessStartMusic, 1);
                        MusicType = SUCCESS_END;
                        break;
                    case SUCCESS_END:
                        Mix_PlayMusic(gSuccessEndMusic, 1);
                        HeroState = SUCCESS;
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
            int vel_i = 0, vel_j = 0;

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
                        case SDLK_q:
                            GameRunning = 0;
                            LevelRunning = 0;
                            break;
                        case SDLK_UP:
                            vel_i = -1;
                            vel_j = 0;
                            break;
                        case SDLK_DOWN:
                            vel_i = 1;
                            vel_j = 0;
                            break;
                        case SDLK_LEFT:
                            HeroState = LEFT;
                            vel_i = 0;
                            vel_j = -1;
                            break;
                        case SDLK_RIGHT:
                            HeroState = RIGHT;
                            vel_i = 0;
                            vel_j = 1;
                            break;
                        default:
                            vel_i = 0;
                            vel_j = 0;
                            break;
                    }
            }

            if (inBorders(cur_level.hero.i, cur_level.hero.j, vel_i, vel_j) &&
                cur_level.field[cur_level.hero.i + vel_i][cur_level.hero.j + vel_j].type == EMPTY) {
                cur_level.hero.i += vel_i;
                cur_level.hero.j += vel_j;
                cur_level.hero.x += STEP * vel_j;
                cur_level.hero.y += STEP * vel_i;
            } else if (inBorders(cur_level.hero.i, cur_level.hero.j, vel_i * 2, vel_j * 2) &&
                       cur_level.field[cur_level.hero.i + vel_i][cur_level.hero.j + vel_j].type == BOX &&
                       cur_level.field[cur_level.hero.i + vel_i * 2][cur_level.hero.j + vel_j * 2].type == EMPTY) {
                cur_level.hero.i += vel_i;
                cur_level.hero.j += vel_j;
                cur_level.field[cur_level.hero.i][cur_level.hero.j].type = EMPTY;
                cur_level.field[cur_level.hero.i + vel_i][cur_level.hero.j + vel_j].type = BOX;
                cur_level.hero.x += STEP * vel_j;
                cur_level.hero.y += STEP * vel_i;
            }

            for (int n = 0; n < cur_level.n_boxes; ++n)
                renderTexture(&gFieldTextures, cur_level.dst[n].x, cur_level.dst[n].y, FieldT[DST], gRenderer);

            int progress = RenderField(&cur_level);
            if (progress == cur_level.n_boxes && MusicType == LEVEL) {
                MusicType = SUCCESS_START;
                Mix_HaltMusic();
                /* TODO(Talkasi): add menu */
            }

            renderTexture(&gHeroTextures, cur_level.hero.x, cur_level.hero.y, HeroT[HeroState], gRenderer);

            SDL_RenderPresent(gRenderer);
            delay_time = MSEC_IN_SEC / FPS - start + SDL_GetTicks();
            if (delay_time > 0)
                SDL_Delay(delay_time);
        }
    }

    return 0;
}

int inBorders(int curPos_i, int curPos_j, int curVel_i, int curVel_j)
{
    return curPos_i + curVel_i > 0 && curPos_i + curVel_i < N_FIELDS_WIDTH &&
           curPos_j + curVel_j > 0 && curPos_j + curVel_j < N_FIELDS_HEIGHT;
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

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        printf("SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError());
        return 0;
    }

    return 1;
}

int loadMedia()
{
    Mix_VolumeMusic(MIX_MAX_VOLUME / 10);
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

    if (!loadTextureFromFile(&gHeroTextures, "./images/hero.bmp", gRenderer, STEP, STEP)) {
        printf("Texture image loading Error.\n");
        return 0;
    }
    setTextures(HeroT, N_HERO_TYPES, STEP, STEP);

    if (!loadTextureFromFile(&gFieldTextures, "./images/fields.bmp", gRenderer, STEP, STEP)) {
        printf("Texture image loading Error.\n");
        return 0;
    }
    setTextures(FieldT, N_FIELD_TYPES, STEP, STEP);

    return 1;
}

int setTextures(SDL_Rect Textures[], int n, int w, int h)
{
    for (int i = 0; i < n; ++i) {
        Textures[i].x = i * w;
        Textures[i].y = 0;
        Textures[i].w = w;
        Textures[i].h = h;
    }
}

int RenderField(struct level *cur_level)
{
    int progress = 0;
    for (int y = 0; y < N_FIELDS_HEIGHT; ++y)
        for (int x = 0; x < N_FIELDS_WIDTH; ++x) {
            switch (cur_level->field[y][x].type) {
                case BOX:
                    for (int dst = 0; dst < cur_level->n_boxes; ++dst)
                        if (cur_level->field[y][x].x == cur_level->dst[dst].x && cur_level->field[y][x].y == cur_level->dst[dst].y) {
                            ++progress;
                            /* TODO(Talkasi): render box on dst */
                            continue;
                        }

                    renderTexture(&gFieldTextures, cur_level->field[y][x].x, cur_level->field[y][x].y, FieldT[BOX], gRenderer);
                    break;
                case WALL:
                    renderTexture(&gFieldTextures, cur_level->field[y][x].x, cur_level->field[y][x].y, FieldT[WALL], gRenderer);
                    break;
                default:
                    break;
            }
        }

    return progress;
}

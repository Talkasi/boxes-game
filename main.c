#include "levels.h"
#include "textures.h"
#include "config.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>

#define MSEC_IN_SEC 1000

int init();
int loadMedia();
int RenderField(struct level *cur_level);
int renderMenu(SDL_Event *e, int *gState, int *lState);
int renderLevel(SDL_Event *e, struct level *cur_level, int *gState, int *lState, int *mState, int *hState);
int inBorders(int curPos_i, int curPos_j, int curVel_i, int curVel_j);
int canMove(int subFieldType);
int setTextures(SDL_Rect Textures[], int n, int w, int h);

enum hero_state { RIGHT,
                  LEFT,
                  SUCCESS,
                  N_HERO_TYPES };

enum music_type {
    NO_MUSIC,
    LEVEL_MUSIC,
    SUCCESS_START,
    SUCCESS_END
};

enum game_state {
    GAME_MENU,
    LEVEL
};

SDL_Window *gWindow = NULL;
SDL_Renderer *gRenderer = NULL;

struct LTexture gHeroTextures;
SDL_Rect HeroT[N_HERO_TYPES];

struct LTexture gFieldTextures;
SDL_Rect FieldT[N_FIELD_TYPES];

struct fonts {
    TTF_Font *lazy;
    TTF_Font *steelpla;
} gFonts;

struct LTexture gTextTexture;

Mix_Music *gMusic = NULL;
Mix_Music *gSuccessStartMusic = NULL;
Mix_Music *gSuccessEndMusic = NULL;

int main(int argc, char *args[])
{
    if (!init()) {
        printf("Failed to initialize SDL.\n");
        return 1;
    }

    if (!loadMedia()) {
        printf("Failed to load media.\n");
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
        int GameState = GAME_MENU;
        int MusicType = LEVEL_MUSIC;
        int HeroState = RIGHT;

        while (LevelRunning) {
            uint32_t start, delay_time;
            start = SDL_GetTicks();

            if (GameState == LEVEL && Mix_PlayingMusic() == 0)
                switch (MusicType) {
                    case LEVEL_MUSIC:
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

            switch (GameState) {
                case (GAME_MENU):
                    GameState = renderMenu(&e, &GameRunning, &LevelRunning);
                    break;
                case (LEVEL):
                    renderLevel(&e, &cur_level, &GameRunning, &LevelRunning, &MusicType, &HeroState);
                    break;
                default:
                    break;
            }

            SDL_RenderPresent(gRenderer);
            delay_time = MSEC_IN_SEC / FPS - start + SDL_GetTicks();
            if (delay_time > 0)
                SDL_Delay(delay_time);
        }
    }

    return 0;
}

int renderMenu(SDL_Event *e, int *gState, int *lState)
{
    int mState = 0;
    while (SDL_PollEvent(e) != 0) {
        if (e->type == SDL_QUIT) {
            *gState = 0;
            *lState = 0;
            break;
        }

        if (e->type == SDL_KEYDOWN)
            switch (e->key.keysym.sym) {
                case SDLK_q:
                    *gState = 0;
                    *lState = 0;
                    break;
                case SDLK_UP:
                    mState = LEVEL;
                    break;
                case SDLK_DOWN:
                    mState = GAME_MENU;
                    break;
                default:
                    break;
            }
    }

    renderTexture(&gHeroTextures,
                  SCREEN_WIDTH / 2 - gHeroTextures.Width / 2,
                  SCREEN_HEIGHT / 2 - gHeroTextures.Height / 2,
                  HeroT[RIGHT], gRenderer);
    SDL_Rect textRect = {SCREEN_WIDTH / 2 - 100 / 2, SCREEN_HEIGHT * 3 / 4, 100, 24};
    renderText("Menu", textRect, gFonts.steelpla, gRenderer);
    return mState;
}

int inBorders(int curPos_i, int curPos_j, int curVel_i, int curVel_j)
{
    return curPos_i + curVel_i > 0 && curPos_i + curVel_i < N_FIELDS_WIDTH &&
           curPos_j + curVel_j > 0 && curPos_j + curVel_j < N_FIELDS_HEIGHT;
}

int canMove(int subFieldType)
{
    return subFieldType == EMPTY || subFieldType == DST;
}

int isBox(int subFieldType)
{
    return subFieldType == N_BOX || subFieldType == D_BOX;
}

int renderLevel(SDL_Event *e, struct level *cur_level, int *gState, int *lState, int *mState, int *hState)
{
    int vel_i = 0, vel_j = 0;

    while (SDL_PollEvent(e) != 0) {
        if (e->type == SDL_QUIT) {
            *gState = 0;
            *lState = 0;
            break;
        }

        if (*mState == LEVEL_MUSIC && e->type == SDL_KEYDOWN)
            switch (e->key.keysym.sym) {
                case SDLK_r:
                    *lState = 0;
                    break;
                case SDLK_q:
                    *gState = 0;
                    *lState = 0;
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
                    *hState = LEFT;
                    vel_i = 0;
                    vel_j = -1;
                    break;
                case SDLK_RIGHT:
                    *hState = RIGHT;
                    vel_i = 0;
                    vel_j = 1;
                    break;
                default:
                    vel_i = 0;
                    vel_j = 0;
                    break;
            }
    }

    if (inBorders(cur_level->hero.i, cur_level->hero.j, vel_i, vel_j) &&
        canMove(cur_level->field[cur_level->hero.i + vel_i][cur_level->hero.j + vel_j])) {
        cur_level->hero.i += vel_i;
        cur_level->hero.j += vel_j;
    } else if (inBorders(cur_level->hero.i, cur_level->hero.j, vel_i * 2, vel_j * 2) &&
               isBox(cur_level->field[cur_level->hero.i + vel_i][cur_level->hero.j + vel_j]) &&
               canMove(cur_level->field[cur_level->hero.i + vel_i * 2][cur_level->hero.j + vel_j * 2])) {
        cur_level->hero.i += vel_i;
        cur_level->hero.j += vel_j;

        if (cur_level->field[cur_level->hero.i][cur_level->hero.j] == N_BOX)
            cur_level->field[cur_level->hero.i][cur_level->hero.j] = EMPTY;
        else if (cur_level->field[cur_level->hero.i][cur_level->hero.j] == D_BOX)
            cur_level->field[cur_level->hero.i][cur_level->hero.j] = DST;

        if (cur_level->field[cur_level->hero.i + vel_i][cur_level->hero.j + vel_j] == DST)
            cur_level->field[cur_level->hero.i + vel_i][cur_level->hero.j + vel_j] = D_BOX;
        else if (cur_level->field[cur_level->hero.i + vel_i][cur_level->hero.j + vel_j] == EMPTY)
            cur_level->field[cur_level->hero.i + vel_i][cur_level->hero.j + vel_j] = N_BOX;
    }

    int progress = RenderField(cur_level);
    if (progress == cur_level->n_boxes && *mState == LEVEL_MUSIC) {
        *mState = SUCCESS_START;
        Mix_HaltMusic();
    }

    renderTexture(&gHeroTextures,
                  cur_level->hero.j * STEP + cur_level->OffsetW,
                  cur_level->hero.i * STEP + cur_level->OffsetH,
                  HeroT[*hState], gRenderer);
}

int init()
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL init Error: %s\n", SDL_GetError());
        return 0;
    }

    if (TTF_Init() == -1) {
        printf("TTF init Error: %s\n", TTF_GetError());
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
    Mix_VolumeMusic((int) (MIX_MAX_VOLUME * 0.1));
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
        printf("Texture loading Error.\n");
        return 0;
    }
    setTextures(HeroT, N_HERO_TYPES, STEP, STEP);

    if (!loadTextureFromFile(&gFieldTextures, "./images/fields_new.png", gRenderer, STEP, STEP)) {
        printf("Texture loading Error.\n");
        return 0;
    }
    setTextures(FieldT, N_FIELD_TYPES, STEP, STEP);

    gFonts.steelpla = TTF_OpenFont("./fonts/steelpla.ttf", 24);
    gFonts.lazy = TTF_OpenFont("./fonts/lazy.ttf", 24);
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
    int x = cur_level->OffsetW;
    int y = cur_level->OffsetH;
    for (int i = 0; i < N_FIELDS_HEIGHT; ++i) {
        for (int j = 0; j < N_FIELDS_WIDTH; ++j) {
            renderTexture(&gFieldTextures, x, y, FieldT[cur_level->field[i][j]], gRenderer);
            x += STEP;
            if (cur_level->field[i][j] == D_BOX)
                ++progress;
        }
        x = cur_level->OffsetW;
        y += STEP;
    }
    return progress;
}

#include "config.h"
#include "levels.h"
#include "textures.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <stdlib.h>

#define MSEC_IN_SEC 1000

int init();
int loadMedia();

int renderField(struct lvl *cur_level);
void renderStart(SDL_Event *e, int *gState);
int renderMenu(SDL_Event *e, int *gState, int *lState);
void renderLvlMenu(SDL_Event *e, int *lvl_n, int *gState);
void renderLvlItems(int OffsetW, int OffsetH);
int renderLvl(SDL_Event *e, struct lvl *cur_level, int *hState, int *gState);

int inBorders(int curPos_i, int curPos_j, int curVel_i, int curVel_j);
int canMove(int subFieldType);

int setTextures(SDL_Rect Textures[], int n, int w, int h);

enum hero_state { RIGHT,
                  LEFT,
                  SUCCESS,
                  N_HERO_TYPES };

enum game_state {
    QUIT,
    START,
    MENU,
    CHOOSE_LVL,
    GET_LVL,
    LVL,
    COMPLETED
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
struct LTexture gStartTexture;

#define INIT_ERR 1
#define LOAD_ERR 2

int main(int argc, char *args[])
{
    if (!init())
        return INIT_ERR;

    if (!loadMedia())
        return LOAD_ERR;

    struct lvl_info lvl_info;
    lvl_info.cur_n = 1;
    lvl_info.info[0] = 1;
    struct lvl cur_level;
    getLvl(lvl_info.cur_n, &cur_level);
    int HeroState = RIGHT;

    SDL_Event e;
    int GameState = START;

    while (GameState) {
        uint32_t start, delay_time;
        start = SDL_GetTicks();

        SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
        SDL_RenderClear(gRenderer);

        switch (GameState) {
            case (START):
                renderStart(&e, &GameState);
                break;
            case (CHOOSE_LVL):
                renderLvlMenu(&e, &lvl_info.cur_n, &GameState);
                break;
            case (LVL):
                lvl_info.cur_n += renderLvl(&e, &cur_level, &HeroState, &GameState);
                if (lvl_info.cur_n > N_LEVELS)
                    GameState = COMPLETED;
                break;
            case (GET_LVL):
                getLvl(lvl_info.cur_n, &cur_level);
                HeroState = RIGHT;
                GameState = LVL;
                break;
            default:
                break;
        }

        SDL_RenderPresent(gRenderer);
        delay_time = MSEC_IN_SEC / FPS - start + SDL_GetTicks();
        if (delay_time > 0)
            SDL_Delay(delay_time);
    }

    return 0;
}

void renderStart(SDL_Event *e, int *gState)
{
    while (SDL_PollEvent(e) != 0) {
        if (e->type == SDL_QUIT) {
            *gState = QUIT;
            break;
        }

        if (e->type == SDL_KEYDOWN)
            switch (e->key.keysym.sym) {
                case SDLK_q:
                    *gState = QUIT;
                    break;
                case SDLK_RETURN:
                case SDLK_KP_ENTER:
                    *gState = CHOOSE_LVL;
                    break;
                default:
                    break;
            }
    }

    // TODO(Talkasi): Press enter to continue image
    renderTexture(&gStartTexture, 0, 0, NULL, gRenderer);
}

void renderLvlMenu(SDL_Event *e, int *lvl_n, int *gState)
{
    int OffsetW = (SCREEN_WIDTH - N_COLS * ITEMS_WIDTH) / (N_COLS + 1);
    int OffsetH = (SCREEN_WIDTH - N_ROWS * ITEMS_WIDTH) / (N_ROWS + 1);
    int x = OffsetW + (OffsetW + ITEMS_WIDTH) * ((*lvl_n - 1) % N_COLS) - ITEMS_WIDTH / 2;
    int y = OffsetH + (OffsetH + ITEMS_WIDTH) * ((*lvl_n - 1) / N_ROWS);

    while (SDL_PollEvent(e) != 0) {
        if (e->type == SDL_QUIT) {
            *gState = QUIT;
            break;
        }

        if (e->type == SDL_KEYDOWN)
            switch (e->key.keysym.sym) {
                case SDLK_q:
                    *gState = QUIT;
                    break;
                case SDLK_RETURN:
                case SDLK_KP_ENTER:
                    *gState = GET_LVL;
                    break;
                case SDLK_UP:
                    if (*lvl_n > N_COLS)
                        *lvl_n -= N_COLS;
                    break;
                case SDLK_DOWN:
                    if (*lvl_n + N_COLS <= N_LEVELS)
                        *lvl_n += N_COLS;
                    break;
                case SDLK_LEFT:
                    if (*lvl_n > 1)
                        *lvl_n -= 1;
                    break;
                case SDLK_RIGHT:
                    if (*lvl_n < N_LEVELS)
                        *lvl_n += 1;
                    break;
                default:
                    break;
            }
    }

    SDL_SetRenderDrawColor(gRenderer, 0, 0, 255, 0);
    SDL_Rect dst = {x, y, ITEMS_WIDTH, ITEMS_WIDTH};
    SDL_RenderDrawRect(gRenderer, &dst);

    renderLvlItems(OffsetW, OffsetH);
}

void renderLvlItems(int OffsetW, int OffsetH)
{
    char text[2] = "";
    SDL_Rect textRect = {OffsetW, OffsetH, ITEMS_WIDTH, ITEMS_WIDTH};
    for (int i = 0; i < N_ROWS; ++i) {
        for (int j = 0; j < N_COLS; ++j) {
            if (i * N_COLS + j + 1 > N_LEVELS)
                return;
            sprintf(text, "%d", i * N_COLS + j + 1);
            if ((i * N_COLS + j + 1) / 10 == 0)
                textRect.x -= ITEMS_WIDTH / 2;
            // !!! TODO(Talkasi): render numbers by textures -- 0.44% CPU instead of 0.20
            renderText(text, textRect, gFonts.steelpla, gRenderer);
            textRect.x += ITEMS_WIDTH + OffsetW;
            if ((i * N_COLS + j + 1) / 10 == 0)
                textRect.x += ITEMS_WIDTH / 2;
        }
        textRect.x = OffsetW;
        textRect.y += ITEMS_WIDTH + OffsetH;
    }
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
                    mState = LVL;
                    break;
                case SDLK_DOWN:
                    mState = MENU;
                    break;
                default:
                    break;
            }
    }

    SDL_Rect textRect = {SCREEN_WIDTH / 2 - 100 / 2, SCREEN_HEIGHT * 3 / 4, 100, 24};
    //    renderText("Menu", textRect, gFonts.steelpla, gRenderer);
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

int renderLvl(SDL_Event *e, struct lvl *cur_level, int *hState, int *gState)
{
    int vel_i = 0, vel_j = 0;

    while (SDL_PollEvent(e) != 0) {
        if (e->type == SDL_QUIT) {
            *gState = QUIT;
            break;
        }

        if (e->type == SDL_KEYDOWN)
            switch (e->key.keysym.sym) {
                case SDLK_RETURN:
                case SDLK_KP_ENTER:
                    *gState = GET_LVL;
                    break;
                case SDLK_q:
                    *gState = QUIT;
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

    int progress = renderField(cur_level);

    renderTexture(&gHeroTextures,
                  cur_level->hero.j * STEP + cur_level->OffsetW,
                  cur_level->hero.i * STEP + cur_level->OffsetH,
                  &HeroT[*hState], gRenderer);

    if (progress == cur_level->n_boxes) {
        *gState = GET_LVL;
        return 1;
    }
    return 0;
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

    if (!loadTextureFromFile(&gHeroTextures, "./images/hero.bmp", gRenderer)) {
        printf("Texture loading Error.\n");
        return 0;
    }
    setTextures(HeroT, N_HERO_TYPES, HERO_SIZE, HERO_SIZE);
    gHeroTextures.Height = STEP;
    gHeroTextures.Width = STEP;

    if (!loadTextureFromFile(&gFieldTextures, "./images/fields_new.png", gRenderer)) {
        printf("Texture loading Error.\n");
        return 0;
    }
    setTextures(FieldT, N_FIELD_TYPES, FITEM_SIZE, FITEM_SIZE);
    gFieldTextures.Height = STEP;
    gFieldTextures.Width = STEP;

    gFonts.steelpla = TTF_OpenFont("./fonts/steelpla.ttf", 24);
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

int renderField(struct lvl *cur_level)
{
    int progress = 0;
    int x = cur_level->OffsetW;
    int y = cur_level->OffsetH;
    for (int i = 0; i < N_FIELDS_HEIGHT; ++i) {
        for (int j = 0; j < N_FIELDS_WIDTH; ++j) {
            renderTexture(&gFieldTextures, x, y, &FieldT[cur_level->field[i][j]], gRenderer);
            x += STEP;
            if (cur_level->field[i][j] == D_BOX)
                ++progress;
        }
        x = cur_level->OffsetW;
        y += STEP;
    }
    return progress;
}

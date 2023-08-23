#include "config.h"
#include "levels.h"
#include "textures.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>

#define MSEC_IN_SEC 1000

int init();
int loadMedia();
int setTextures(SDL_Rect Textures[], int n, int w, int h);

int renderField(struct lvl *cur_level);
void renderStart(SDL_Event *e, int *gState);
void renderLvlMenu(SDL_Event *e, int *lvl_n, int *gState);
void renderLvlItems(int OffsetW, int OffsetH);
void renderLvl(SDL_Event *e, struct lvl *cur_level, int *hState, int *n_steps, int *lvl_n, int *gState);

int inBorders(int curPos_i, int curPos_j, int curVel_i, int curVel_j);
int canMove(int subFieldType);

enum hero_state { RIGHT,
                  LEFT,
                  SUCCESS,
                  N_HERO_TYPES };

enum game_state {
    QUIT,
    START,
    CHOOSE_LVL,
    GET_LVL,
    LVL,
    COMPLETED
};

SDL_Window *gWindow = NULL;
SDL_Renderer *gRenderer = NULL;
struct LTexture gHeroTextures;
struct LTexture gFieldTextures;
SDL_Rect HeroT[N_HERO_TYPES];
SDL_Rect FieldT[N_FIELD_TYPES];
TTF_Font *lazy_font;

int main(int argc, char *args[])
{
    int rc;
    if ((rc = init()) != 0 || (rc = loadMedia()) != 0)
        return rc;

    struct lvl_info lvl_info;
    lvl_info.cur_n = 1;
    lvl_info.info[0] = 1;

    int n_steps;
    struct lvl cur_level;
    int HeroState;

    SDL_Event e;
    int GameState = START;
    while (GameState != QUIT) {
        uint32_t start, delay_time;
        start = SDL_GetTicks();

        SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
        SDL_RenderClear(gRenderer);

        switch (GameState) {
            case (LVL):
                renderLvl(&e, &cur_level, &HeroState, &n_steps, &lvl_info.cur_n, &GameState);
                if (lvl_info.cur_n > N_LEVELS)
                    GameState = COMPLETED;
                break;
            case (GET_LVL):
                getLvl(lvl_info.cur_n, &cur_level);
                HeroState = RIGHT;
                GameState = LVL;
                n_steps = 0;
                break;
            case (START):
                renderStart(&e, &GameState);
                break;
            case (CHOOSE_LVL):
                renderLvlMenu(&e, &lvl_info.cur_n, &GameState);
                break;
            case (COMPLETED):
                GameState = QUIT;
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

    char text[STRT_LINE_LENGTH * STRT_NLINES] = " This is my version \n"
                                                "   of boxxle game   \n"
                                                "                    \n"
                                                "Press Enter to enjoy.";

    SDL_Rect Rect = {(SCREEN_WIDTH - STRT_LINE_LENGTH * START_LETTERW) / 2,
                     (SCREEN_HEIGHT - STRT_NLINES * START_LETTERH) / 2,
                     STRT_LINE_LENGTH * START_LETTERW,
                     STRT_NLINES * START_LETTERH};
    renderText(text, Rect, lazy_font, 0, gRenderer);
}

void renderLvlMenu(SDL_Event *e, int *lvl_n, int *gState)
{
    int OffsetW = (SCREEN_WIDTH - LVL_MENU_NCOLS * LVL_NUMBER_SIZE) / (LVL_MENU_NCOLS + 1);
    int OffsetH = (SCREEN_WIDTH - LVL_MENU_NROWS * LVL_NUMBER_SIZE) / (LVL_MENU_NROWS + 1);
    int x = OffsetW + (OffsetW + LVL_NUMBER_SIZE) * ((*lvl_n - 1) % LVL_MENU_NCOLS) -
            LVL_NUMBER_SIZE / 2 - (LVL_POINTER_W - LVL_NUMBER_SIZE) / 2;
    int y = OffsetH + (OffsetH + LVL_NUMBER_SIZE) * ((*lvl_n - 1) / LVL_MENU_NROWS) -
            (LVL_POINTER_H - LVL_NUMBER_SIZE) / 2;

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
                    if (*lvl_n > LVL_MENU_NCOLS)
                        *lvl_n -= LVL_MENU_NCOLS;
                    break;
                case SDLK_DOWN:
                    if (*lvl_n + LVL_MENU_NCOLS <= N_LEVELS)
                        *lvl_n += LVL_MENU_NCOLS;
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

    SDL_SetRenderDrawColor(gRenderer, 0x0, 0x0, 0x0, 0x0);
    SDL_Rect dst = {x, y, LVL_POINTER_W, LVL_POINTER_H};
    SDL_RenderDrawRect(gRenderer, &dst);

    renderLvlItems(OffsetW, OffsetH);
}

void renderLvlItems(int OffsetW, int OffsetH)
{
    char text[2] = "";
    SDL_Rect textRect = {OffsetW, OffsetH, LVL_NUMBER_SIZE, LVL_NUMBER_SIZE};
    for (int i = 0; i < LVL_MENU_NROWS; ++i) {
        for (int j = 0; j < LVL_MENU_NCOLS; ++j) {
            if (i * LVL_MENU_NCOLS + j + 1 > N_LEVELS)
                return;
            sprintf(text, "%d", i * LVL_MENU_NCOLS + j + 1);
            if ((i * LVL_MENU_NCOLS + j + 1) / 10 == 0)
                textRect.x -= LVL_NUMBER_SIZE / 2;
            renderText(text, textRect, lazy_font, 0, gRenderer);
            SDL_RenderDrawRect(gRenderer, &textRect);
            textRect.x += LVL_NUMBER_SIZE + OffsetW;
            if ((i * LVL_MENU_NCOLS + j + 1) / 10 == 0)
                textRect.x += LVL_NUMBER_SIZE / 2;
        }
        textRect.x = OffsetW;
        textRect.y += LVL_NUMBER_SIZE + OffsetH;
    }
}

int inBorders(int curPos_i, int curPos_j, int curVel_i, int curVel_j)
{
    return curPos_i + curVel_i > 0 && curPos_i + curVel_i < NTILES_WIDTH &&
           curPos_j + curVel_j > 0 && curPos_j + curVel_j < NTILES_HEIGHT;
}

int canMove(int subFieldType)
{
    return subFieldType == EMPTY || subFieldType == DST;
}

int isBox(int subFieldType)
{
    return subFieldType == N_BOX || subFieldType == D_BOX;
}

void renderLvlInfo(int lvl_n, int n_steps)
{
    char text[LVLI_LETTERS_NMAX] = "";
    sprintf(text, "Lvl No. %d\nSteps: %d", lvl_n, n_steps);
    SDL_Rect Rect = {SCREEN_WIDTH - LVLI_LETTERS_NMAX * LVLI_LETTERW / LVLI_NLINES, 0,
                     LVLI_LETTERS_NMAX * LVLI_LETTERW / LVLI_NLINES, LVLI_LETTERH * LVLI_NLINES};
    renderText(text, Rect, lazy_font, 0, gRenderer);
}

void renderLvl(SDL_Event *e, struct lvl *cur_level, int *hState, int *n_steps, int *lvl_n, int *gState)
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
                case SDLK_r:
                    *gState = GET_LVL;
                    break;
                case SDLK_q:
                    *gState = QUIT;
                    break;
                case SDLK_m:
                    *gState = CHOOSE_LVL;
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

    if (!(vel_i == 0 && vel_j == 0)) {
        if (inBorders(cur_level->hero.i, cur_level->hero.j, vel_i, vel_j) &&
            canMove(cur_level->field[cur_level->hero.i + vel_i][cur_level->hero.j + vel_j])) {
            cur_level->hero.i += vel_i;
            cur_level->hero.j += vel_j;
            ++*n_steps;
        } else if (inBorders(cur_level->hero.i, cur_level->hero.j, vel_i * 2, vel_j * 2) &&
                   isBox(cur_level->field[cur_level->hero.i + vel_i][cur_level->hero.j + vel_j]) &&
                   canMove(cur_level->field[cur_level->hero.i + vel_i * 2][cur_level->hero.j + vel_j * 2])) {
            cur_level->hero.i += vel_i;
            cur_level->hero.j += vel_j;
            ++*n_steps;

            if (cur_level->field[cur_level->hero.i][cur_level->hero.j] == N_BOX)
                cur_level->field[cur_level->hero.i][cur_level->hero.j] = EMPTY;
            else if (cur_level->field[cur_level->hero.i][cur_level->hero.j] == D_BOX)
                cur_level->field[cur_level->hero.i][cur_level->hero.j] = DST;

            if (cur_level->field[cur_level->hero.i + vel_i][cur_level->hero.j + vel_j] == DST)
                cur_level->field[cur_level->hero.i + vel_i][cur_level->hero.j + vel_j] = D_BOX;
            else if (cur_level->field[cur_level->hero.i + vel_i][cur_level->hero.j + vel_j] == EMPTY)
                cur_level->field[cur_level->hero.i + vel_i][cur_level->hero.j + vel_j] = N_BOX;
        }
    }

    int progress = renderField(cur_level);

    renderTexture(&gHeroTextures,
                  cur_level->hero.j * STEP + cur_level->OffsetW,
                  cur_level->hero.i * STEP + cur_level->OffsetH,
                  &HeroT[*hState], gRenderer);

    renderLvlInfo(*lvl_n, *n_steps);
    if (progress == cur_level->n_boxes) {
        *gState = GET_LVL;
        ++*lvl_n;
    }
}

int renderField(struct lvl *cur_level)
{
    int progress = 0;
    int x = cur_level->OffsetW;
    int y = cur_level->OffsetH;
    for (int i = 0; i < NTILES_HEIGHT; ++i) {
        for (int j = 0; j < NTILES_WIDTH; ++j) {
            renderTexture(&gFieldTextures, x, y, &FieldT[cur_level->field[i][j]], gRenderer);
            x += TILE_SIZE;
            if (cur_level->field[i][j] == D_BOX)
                ++progress;
        }
        x = cur_level->OffsetW;
        y += TILE_SIZE;
    }
    return progress;
}

int init()
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL_INIT_ERR: %s\n", SDL_GetError());
        return SDL_INIT_ERR;
    }

    if (TTF_Init() == -1) {
        printf("TTF_INIT_ERR: %s\n", TTF_GetError());
        return TTF_INIT_ERR;
    }

    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");
    gWindow = SDL_CreateWindow("Boxes Game",
                               SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                               SCREEN_WIDTH, SCREEN_HEIGHT,
                               SDL_WINDOW_SHOWN);
    if (gWindow == NULL) {
        printf("SDL_WINDOW_ERR: %s\n", SDL_GetError());
        return SDL_WINDOW_ERR;
    }

    gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED);
    if (gRenderer == NULL) {
        printf("SDL_RENDERER_ERR: %s\n", SDL_GetError());
        return SDL_RENDERER_ERR;
    }

    SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0x0);
    int imgFlags = IMG_INIT_PNG;
    if (!(IMG_Init(imgFlags) & imgFlags)) {
        printf("IMG_INIT_ERR: %s\n", IMG_GetError());
        return IMG_INIT_ERR;
    }

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        printf("MIXER_INIT_ERR: %s\n", Mix_GetError());
        return MIXER_INIT_ERR;
    }

    return 0;
}

int loadMedia()
{
    Mix_VolumeMusic((int) (MIX_MAX_VOLUME * 0.1));

    int rc;
    if ((rc = loadTextureFromFile(&gHeroTextures, "./images/hero.bmp", gRenderer)) != 0)
        return rc;

    setTextures(HeroT, N_HERO_TYPES, HERO_SIZE, HERO_SIZE);
    gHeroTextures.Height = TILE_SIZE;
    gHeroTextures.Width = TILE_SIZE;

    if ((rc = loadTextureFromFile(&gFieldTextures, "./images/fields_new.png", gRenderer)) != 0)
        return rc;

    setTextures(FieldT, N_FIELD_TYPES, TILE_SIZE, TILE_SIZE);
    gFieldTextures.Height = TILE_SIZE;
    gFieldTextures.Width = TILE_SIZE;

    lazy_font = TTF_OpenFont("./fonts/lazy.ttf", 24);
    return 0;
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

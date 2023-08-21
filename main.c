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

int renderField(struct lvl *cur_level);
void renderStart(SDL_Event *e, int *gState);
void renderLvlMenu(SDL_Event *e, int *lvl_n, int *gState);
void renderLvlItems(int OffsetW, int OffsetH);
void renderLvl(SDL_Event *e, struct lvl *cur_level, int *hState, int *n_steps, int *lvl_n, int *gState);

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

    int n_steps;
    struct lvl cur_level;
    int HeroState;

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

    // TODO(Talkasi): Press enter to continue image
    renderTexture(&gStartTexture, 0, 0, NULL, gRenderer);
}

#define LVL_POINTER_W (ITEM_SIDE_LENGTH * 6 / 5)
#define LVL_POINTER_H (ITEM_SIDE_LENGTH * 3 / 2)

void renderLvlMenu(SDL_Event *e, int *lvl_n, int *gState)
{
    int OffsetW = (SCREEN_WIDTH - N_COLS * ITEM_SIDE_LENGTH) / (N_COLS + 1);
    int OffsetH = (SCREEN_WIDTH - N_ROWS * ITEM_SIDE_LENGTH) / (N_ROWS + 1);
    int x = OffsetW + (OffsetW + ITEM_SIDE_LENGTH) * ((*lvl_n - 1) % N_COLS) - ITEM_SIDE_LENGTH / 2 - (LVL_POINTER_W - ITEM_SIDE_LENGTH) / 2;
    int y = OffsetH + (OffsetH + ITEM_SIDE_LENGTH) * ((*lvl_n - 1) / N_ROWS) - (LVL_POINTER_H - ITEM_SIDE_LENGTH) / 2;

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

    SDL_SetRenderDrawColor(gRenderer, 0x0, 0x0, 0x0, 0x0);
    SDL_Rect dst = {x, y, LVL_POINTER_W, LVL_POINTER_H};
    SDL_RenderDrawRect(gRenderer, &dst);

    renderLvlItems(OffsetW, OffsetH);
}

void renderLvlItems(int OffsetW, int OffsetH)
{
    char text[2] = "";
    int rectW = ITEM_SIDE_LENGTH;
    int rectH = ITEM_SIDE_LENGTH;
    SDL_Rect textRect = {OffsetW, OffsetH, rectW, rectH};
    for (int i = 0; i < N_ROWS; ++i) {
        for (int j = 0; j < N_COLS; ++j) {
            if (i * N_COLS + j + 1 > N_LEVELS)
                return;
            sprintf(text, "%d", i * N_COLS + j + 1);
            if ((i * N_COLS + j + 1) / 10 == 0)
                textRect.x -= rectW / 2;
            renderText(text, textRect, gFonts.steelpla, gRenderer);
            SDL_RenderDrawRect(gRenderer, &textRect);
            textRect.x += rectW + OffsetW;
            if ((i * N_COLS + j + 1) / 10 == 0)
                textRect.x += rectW / 2;
        }
        textRect.x = OffsetW;
        textRect.y += rectH + OffsetH;
    }
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

#define LETTERS_MAX 11
#define LETTER_HEIGHT 30
#define LETTER_WIDTH 20
void renderLvlInfo(int lvl_n, int n_steps)
{
    char text[LETTERS_MAX] = "";
    sprintf(text, "Lvl No. %d", lvl_n);
    int text_w = (int) strlen(text) * LETTER_WIDTH;
    SDL_Rect Rect = {SCREEN_WIDTH - text_w, 0, text_w, LETTER_HEIGHT};
    renderText(text, Rect, gFonts.steelpla, gRenderer);

    sprintf(text, "Steps: %d", n_steps);
    text_w = (int) strlen(text) * LETTER_WIDTH;
    Rect.x = SCREEN_WIDTH - text_w;
    Rect.y = LETTER_HEIGHT;
    Rect.w = text_w;
    renderText(text, Rect, gFonts.steelpla, gRenderer);
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

    SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0);

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

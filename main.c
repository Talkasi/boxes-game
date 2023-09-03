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
void renderLvlItems(int lvl_n);
void renderLvl(SDL_Event *e, struct lvl *cur_level, int *hState, int *volume, int *n_steps, int *lvl_n, int *gState);

int onLevelNumber(int x, int y);
int inBorders(int curPos_i, int curPos_j, int curVel_i, int curVel_j);
int canMove(int subFieldType);

enum hero_state { H_RIGHT,
                  H_LEFT,
                  H_SUCCESS,
                  N_HERO_TYPES };

enum icons {
    I_MENU,
    I_RESTART,
    I_SOUND,
    I_VOLUME,
    N_ICON_TYPES
};

enum game_state {
    G_QUIT,
    G_START,
    G_LVL_MENU,
    G_GET_LVL,
    G_LVL,
    G_COMPLETED
};

enum music_state {
    M_MAIN,
    M_LEVEL,
    M_SUCCESS_S,
    M_SUCCESS_E
};

SDL_Window *gWindow = NULL;
SDL_Renderer *gRenderer = NULL;
struct LTexture gStartTexture;
struct LTexture gHeroTextures;
struct LTexture gFieldTextures;
struct LTexture gIconTextures;
struct LTexture gLvlNTextures;
SDL_Rect HeroT[N_HERO_TYPES];
SDL_Rect FieldT[N_FIELD_TYPES];
SDL_Rect IconT[N_ICON_TYPES];
SDL_Rect LvlNT[N_LEVELS];
TTF_Font *lazy_font;

struct music {
    int MusicState;
    Mix_Music *main;
    Mix_Music *level;
    Mix_Music *success_start;
    Mix_Music *success_end;
} gMusic;

#define MUSIC_INFINITY (-1)
void playMusic(int music_key, int n_times, Mix_Music *music)
{
    gMusic.MusicState = music_key;
    Mix_PlayMusic(music, n_times);
}

int main(int argc, char *args[])
{
    int rc;
    if ((rc = init()) != 0 || (rc = loadMedia()) != 0)
        return rc;

    int lvl_n = 1;
    int volume = 50;
    int n_steps;
    struct lvl cur_level;
    int HeroState;

    Mix_VolumeMusic((int) (MIX_MAX_VOLUME * volume * VOLUME_CALIBRATION / 100));
    playMusic(M_MAIN, MUSIC_INFINITY, gMusic.main);

    SDL_Event e;
    int GameState = G_START;
    while (GameState != G_QUIT) {
        uint32_t start, delay_time;
        start = SDL_GetTicks();

        SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
        SDL_RenderClear(gRenderer);

        switch (GameState) {
            case (G_LVL):
                renderLvl(&e, &cur_level, &HeroState, &volume, &n_steps, &lvl_n, &GameState);
                if (lvl_n > N_LEVELS)
                    GameState = G_COMPLETED;
                break;
            case (G_GET_LVL):
                getLvl(lvl_n, &cur_level);
                HeroState = H_RIGHT;
                GameState = G_LVL;
                n_steps = 0;
                break;
            case (G_LVL_MENU):
                if (gMusic.MusicState != M_MAIN)
                    playMusic(M_MAIN, MUSIC_INFINITY, gMusic.main);
                renderLvlMenu(&e, &lvl_n, &GameState);
                break;
            case (G_START):
                renderStart(&e, &GameState);
                break;
            case (G_COMPLETED):
                GameState = G_QUIT;
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
            *gState = G_QUIT;
            break;
        }

        if (e->type == SDL_KEYDOWN)
            switch (e->key.keysym.sym) {
                case SDLK_q:
                    *gState = G_QUIT;
                    break;
                case SDLK_RETURN:
                case SDLK_KP_ENTER:
                    *gState = G_LVL_MENU;
                    freeTexture(&gStartTexture);
                    break;
                default:
                    break;
            }
    }

    renderTexture(&gStartTexture, (SCREEN_WIDTH - START_SIZE) / 2, (SCREEN_HEIGHT - START_SIZE) / 2, NULL, gRenderer);
}

void handleGetLvl(int *gState)
{
    *gState = G_GET_LVL;
    if (gMusic.MusicState != M_LEVEL)
        playMusic(M_LEVEL, MUSIC_INFINITY, gMusic.level);
}
void renderLvlMenu(SDL_Event *e, int *lvl_n, int *gState)
{
    char text[] = "Level Menu";
    int text_len = (int) strlen(text);
    SDL_Rect textRect = {(SCREEN_WIDTH - text_len * LVL_NUMBER_SIZE) / 2, MENU_POSY, LVL_NUMBER_SIZE * text_len, LVL_NUMBER_SIZE};
    renderText(text, textRect, lazy_font, 0, gRenderer);

    while (SDL_PollEvent(e) != 0) {
        if (e->type == SDL_QUIT) {
            *gState = G_QUIT;
            break;
        }

        if (e->type == SDL_MOUSEBUTTONDOWN) {
            if (e->button.button == SDL_BUTTON_LEFT) {
                int x, y, rc;
                SDL_GetMouseState(&x, &y);
                if ((rc = onLevelNumber(x, y)) != 0) {
                    *lvl_n = rc;
                }
            }
        }

        if (e->type == SDL_KEYDOWN)
            switch (e->key.keysym.sym) {
                case SDLK_q:
                    *gState = G_QUIT;
                    break;
                case SDLK_RETURN:
                case SDLK_KP_ENTER:
                    handleGetLvl(gState);
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

    renderLvlItems(*lvl_n);
}

int onLevelNumber(int x, int y)
{
    if ((y - MENU_OFFSET) % (LVL_MENU_OFFSETH + LVL_NUMBER_SIZE) >= LVL_MENU_OFFSETH && x % (LVL_MENU_OFFSETW + LVL_NUMBER_SIZE) >= LVL_MENU_OFFSETW)
        return (y - MENU_OFFSET) / (LVL_MENU_OFFSETH + LVL_NUMBER_SIZE) * LVL_MENU_NCOLS + x / (LVL_MENU_OFFSETW + LVL_NUMBER_SIZE) + 1;
    else
        return 0;
}

void renderLvlItems(int lvl_n)
{
    int x = LVL_MENU_OFFSETW;
    int y = LVL_MENU_OFFSETH + MENU_OFFSET;

    for (int i = 0; i < LVL_MENU_NROWS; ++i) {
        for (int j = 0; j < LVL_MENU_NCOLS && i * LVL_MENU_NCOLS + j + 1 <= N_LEVELS; ++j) {
            renderTexture(&gLvlNTextures, x, y, &LvlNT[i * LVL_MENU_NCOLS + j], gRenderer);

            if (i * LVL_MENU_NCOLS + j + 1 == lvl_n) {
                SDL_SetRenderDrawColor(gRenderer, 0x0, 0x0, 0x0, 0x0);
                SDL_Rect dst = {x - (LVL_POINTER_W - LVL_NUMBER_SIZE) / 2,
                                y - (LVL_POINTER_H - LVL_NUMBER_SIZE) / 2,
                                LVL_POINTER_W, LVL_POINTER_H};
                SDL_RenderDrawRect(gRenderer, &dst);
            }

            x += LVL_NUMBER_SIZE + LVL_MENU_OFFSETW;
        }
        x = LVL_MENU_OFFSETW;
        y += LVL_NUMBER_SIZE + LVL_MENU_OFFSETH;
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

void renderLvlInfo(int lvl_n, int volume, int n_steps)
{
    char text[LVLI_LETTERS_NMAX] = "";
    sprintf(text, "Lvl No. %d\nSteps: %d", lvl_n, n_steps);
    SDL_Rect Rect = {SCREEN_WIDTH - LVLI_LETTERS_NMAX * LVLI_LETTERW / LVLI_NLINES, 0,
                     LVLI_LETTERS_NMAX * LVLI_LETTERW / LVLI_NLINES, LVLI_LETTERH * LVLI_NLINES};
    renderText(text, Rect, lazy_font, 0, gRenderer);

    renderTexture(&gIconTextures, I_MENU_POSX, 0, &IconT[I_MENU], gRenderer);
    renderTexture(&gIconTextures, I_RESTART_POSX, 0, &IconT[I_RESTART], gRenderer);
    renderTexture(&gIconTextures, I_SOUND_POSX, 0, &IconT[I_SOUND], gRenderer);

    SDL_SetRenderDrawColor(gRenderer, 0x0, 0x0, 0x0, 0x0);
    SDL_RenderDrawLine(gRenderer, VOLUME_LINE_STRT, TILE_SIZE / 2, VOLUME_LINE_END, TILE_SIZE / 2);
    renderTexture(&gIconTextures,
                  VOLUME_LINE_STRT + (int) (volume / 100.0 * (VOLUME_LINE_END - VOLUME_LINE_STRT)) - TILE_SIZE / 2, 0,
                  &IconT[I_VOLUME], gRenderer);
}


void changeSoundVolume(int *volume, int x)
{
    *volume = (x - VOLUME_LINE_STRT) * 100 / (VOLUME_LINE_END - VOLUME_LINE_STRT);
    if (*volume < 0)
        *volume = 0;
    else if (*volume > 100)
        *volume = 100;
    Mix_VolumeMusic((int) (*volume * VOLUME_CALIBRATION * MIX_MAX_VOLUME / 100));
}

void handleMouseButton(SDL_Event *e, int *volume, int *gState)
{
    static int dot_pressed = 0;
    static int comf_volume = 0;
    int x = 0, y = 0;
    SDL_GetMouseState(&x, &y);
    if (e->type == SDL_MOUSEBUTTONDOWN) {
        if (e->button.button == SDL_BUTTON_LEFT) {
            if (y > ICON_OFFSET && y < TILE_SIZE - ICON_OFFSET) {
                if (x > I_MENU_POSX + ICON_OFFSET && x < I_MENU_POSX + TILE_SIZE - ICON_OFFSET)
                    *gState = G_LVL_MENU;
                else if (x > I_RESTART_POSX + ICON_OFFSET && x < I_RESTART_POSX + TILE_SIZE - ICON_OFFSET)
                    handleGetLvl(gState);
                else if (x > I_SOUND_POSX + ICON_OFFSET && x < I_SOUND_POSX + TILE_SIZE - ICON_OFFSET) {
                    if (*volume != 0) {
                        comf_volume = *volume;
                        *volume = 0;
                    } else
                        *volume = comf_volume;
                    Mix_VolumeMusic((int) (*volume * VOLUME_CALIBRATION * MIX_MAX_VOLUME / 100));
                }
            }

            if (y > DOT_OFFSET && y < TILE_SIZE - DOT_OFFSET &&
                x >= VOLUME_LINE_STRT && x <= VOLUME_LINE_END) {
                dot_pressed = 1;
                changeSoundVolume(volume, x);
            }
        }
    }

    if (dot_pressed)
        changeSoundVolume(volume, x);

    if (e->type == SDL_MOUSEBUTTONUP)
        if (e->button.button == SDL_BUTTON_LEFT)
            if (dot_pressed)
                dot_pressed = 0;
}

void renderLvl(SDL_Event *e, struct lvl *cur_level, int *hState, int *volume, int *n_steps, int *lvl_n, int *gState)
{
    int vel_i = 0, vel_j = 0;

    while (SDL_PollEvent(e) != 0) {
        if (e->type == SDL_QUIT) {
            *gState = G_QUIT;
            break;
        }

        handleMouseButton(e, volume, gState);
        if (e->type == SDL_KEYDOWN)
            switch (e->key.keysym.sym) {
                case SDLK_RETURN:
                case SDLK_KP_ENTER:
                case SDLK_r:
                    handleGetLvl(gState);
                    break;
                case SDLK_q:
                    *gState = G_QUIT;
                    break;
                case SDLK_m:
                    *gState = G_LVL_MENU;
                case SDLK_UP:
                    vel_i = -1;
                    vel_j = 0;
                    break;
                case SDLK_DOWN:
                    vel_i = 1;
                    vel_j = 0;
                    break;
                case SDLK_LEFT:
                    *hState = H_LEFT;
                    vel_i = 0;
                    vel_j = -1;
                    break;
                case SDLK_RIGHT:
                    *hState = H_RIGHT;
                    vel_i = 0;
                    vel_j = 1;
                    break;
                default:
                    vel_i = 0;
                    vel_j = 0;
                    break;
            }
    }

    if (gMusic.MusicState == M_LEVEL && !(vel_i == 0 && vel_j == 0)) {
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
    if (progress == cur_level->n_boxes) {
        if (Mix_PlayingMusic() && gMusic.MusicState == M_LEVEL) {
            playMusic(M_SUCCESS_S, 0, gMusic.success_start);
        } else if (!Mix_PlayingMusic() && gMusic.MusicState == M_SUCCESS_S) {
            *hState = H_SUCCESS;
            playMusic(M_SUCCESS_E, 0, gMusic.success_end);
        } else if (!Mix_PlayingMusic() && gMusic.MusicState == M_SUCCESS_E) {
            *hState = H_SUCCESS;
            handleGetLvl(gState);
            ++*lvl_n;
        } else if (Mix_PlayingMusic() && gMusic.MusicState == M_SUCCESS_E)
            *hState = H_SUCCESS;
    }

    renderTexture(&gHeroTextures,
                  cur_level->hero.j * STEP + cur_level->OffsetW,
                  cur_level->hero.i * STEP + cur_level->OffsetH,
                  &HeroT[*hState], gRenderer);
    renderLvlInfo(*lvl_n, *volume, *n_steps);
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
    gMusic.main = Mix_LoadMUS("./music/main.mp3");
    if (gMusic.main == NULL) {
        printf("MIXER_LOAD_ERR: %s\n", Mix_GetError());
        return MIXER_LOAD_ERR;
    }

    gMusic.level = Mix_LoadMUS("./music/level.mp3");
    if (gMusic.level == NULL) {
        printf("MIXER_LOAD_ERR: %s\n", Mix_GetError());
        return MIXER_LOAD_ERR;
    }

    gMusic.success_start = Mix_LoadMUS("./music/jingle_01.mp3");
    if (gMusic.success_start == NULL) {
        printf("MIXER_LOAD_ERR: %s\n", Mix_GetError());
        return MIXER_LOAD_ERR;
    }

    gMusic.success_end = Mix_LoadMUS("./music/jingle_02.mp3");
    if (gMusic.success_end == NULL) {
        printf("MIXER_LOAD_ERR: %s\n", Mix_GetError());
        return MIXER_LOAD_ERR;
    }

    int rc;
    if ((rc = loadTextureFromFile(&gStartTexture, "./images/start01.png", gRenderer)) != 0)
        return rc;

    gStartTexture.Height = START_SIZE;
    gStartTexture.Width = START_SIZE;

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

    if ((rc = loadTextureFromFile(&gIconTextures, "./images/icons_new.png", gRenderer)) != 0)
        return rc;

    setTextures(IconT, N_ICON_TYPES, TILE_SIZE, TILE_SIZE);
    gIconTextures.Height = TILE_SIZE;
    gIconTextures.Width = TILE_SIZE;

    if ((rc = loadTextureFromFile(&gLvlNTextures, "./images/lvl_numbers.png", gRenderer)) != 0)
        return rc;

    setTextures(LvlNT, N_LEVELS, LVL_NUMBER_SIZE, LVL_NUMBER_SIZE);
    gLvlNTextures.Height = LVL_NUMBER_SIZE;
    gLvlNTextures.Width = LVL_NUMBER_SIZE;

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

#ifndef GAME_CONFIG
#define GAME_CONFIG

#define SCREEN_WIDTH 510
#define SCREEN_HEIGHT 600

#define NTILES_WIDTH 10
#define NTILES_HEIGHT 10

#define STEP (SCREEN_WIDTH / NTILES_WIDTH)
#define HERO_SIZE STEP
#define TILE_SIZE STEP

#define N_LEVELS 6
#define LVL_MENU_NROWS 3
#define LVL_MENU_NCOLS 3
#define LVL_NUMBER_SIZE 30

#define FPS 30

#define START_LETTERH 30
#define START_LETTERW 18
#define STRT_LINE_LENGTH 24
#define STRT_NLINES 4

#define LVL_POINTER_W (LVL_NUMBER_SIZE * 6 / 5)
#define LVL_POINTER_H (LVL_NUMBER_SIZE * 3 / 2)

#define LVLI_LETTERS_NMAX 22
#define LVLI_NLINES 2
#define LVLI_LETTERH 27
#define LVLI_LETTERW 18

#define ICON_SIZE 36
#define ICON_OFFSET ((TILE_SIZE - ICON_SIZE) / 2)
#define DOT_SIZE (ICON_SIZE / 2)
#define DOT_OFFSET ((TILE_SIZE - DOT_SIZE) / 2)

/* Return codes */
#define SDL_INIT_ERR (-1)
#define TTF_INIT_ERR (-2)
#define IMG_INIT_ERR (-3)
#define MIXER_INIT_ERR (-4)
#define SDL_WINDOW_ERR (-5)
#define SDL_RENDERER_ERR (-6)
#define SDL_IMG_LOAD_ERR (-7)
#define SDL_IMG_OPT_ERR (-8)
#define SDL_CRT_TXTR_ERR (-9)
#define MIXER_LOAD_ERR (-10)
#endif
